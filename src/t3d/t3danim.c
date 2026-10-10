/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "t3d/t3danim.h"
#include <malloc.h>


#define SQRT_2_INV 0.70710678118f
#define KF_TIME_TICK (1.0f / 60.0f)

_Static_assert(_Alignof(T3DAnimTargetQuat) >= 8, "T3DAnimTargetQuat must be 8-byte aligned");
_Static_assert(offsetof(T3DAnimTargetQuat, kfCurr) % 8 == 0, "kfCurr must be 8-byte aligned");
_Static_assert(offsetof(T3DAnimTargetQuat, kfNext) % 8 == 0, "kfNext must be 8-byte aligned");

typedef uint64_t __attribute__((may_alias)) u64_alias_t;
typedef uint64_t __attribute__((aligned(2), may_alias)) u64_unaligned_t;

// Maps the input data streamed from the animation data file
typedef struct {
  uint16_t nextTime;
  uint16_t channelIdx;
  uint16_t data[2]; // can be either 1 or 2 16-bit values (scalar / quat)
} T3DAnimKF;

// Starts loading the next part of the file into the given half (async)
static void stream_load(T3DAnim *anim, uint32_t half) {
  if(anim->loadOffset >= anim->streamSize)anim->loadOffset = 0; // prefetch the start again for looping
  uint32_t size = anim->streamSize - anim->loadOffset;
  if(size > anim->bufferHalfSize)size = anim->bufferHalfSize;

  uint8_t *dst = anim->buffer + half * anim->bufferHalfSize;
  data_cache_hit_invalidate(dst, anim->bufferHalfSize);
  anim->dmaTicket = dma_read_raw_async(dst, anim->romAddr + anim->loadOffset, size);
  anim->loadOffset += size;
}

static inline void stream_wait(T3DAnim *anim) {
  if(anim->dmaTicket) {
    dma_wait_finished(anim->dmaTicket);
    anim->dmaTicket = 0;
  }
}

// (Re-)loads the stream from the start, blocks until the first half is loaded
static void stream_reset(T3DAnim *anim) {
  stream_wait(anim);
  anim->loadOffset = 0;
  stream_load(anim, 0);
  stream_wait(anim);
  stream_load(anim, 1);
  anim->readPos = 0;
  anim->streamPos = 0;
}

static void stream_rewind(T3DAnim *anim) {
  const uint32_t halfSize = anim->bufferHalfSize;
  uint32_t halfStart = anim->readPos >= halfSize ? halfSize : 0;
  uint32_t halfOffset = anim->streamPos - (anim->readPos - halfStart); // file offset of the current half
  anim->streamPos = 0;

  // start of the file is in the current half (file smaller than a half, or it was just switched to)
  if(halfOffset == 0 || halfOffset >= anim->streamSize) {
    anim->readPos = halfStart;
    return;
  }
  // current half contains the end of the file, so the other one was loaded from the start (looping)
  if(halfOffset + halfSize >= anim->streamSize) {
    stream_wait(anim);
    anim->readPos = halfSize - halfStart;
    stream_load(anim, halfStart ? 1 : 0);
    return;
  }
  stream_reset(anim);
}

// Copies the next keyframe out of the stream, returns false at the end of the stream
static inline bool stream_read_kf(T3DAnim *anim, T3DAnimKF *kf) {
  uint32_t size = anim->nextKfSize;
  if(anim->streamPos + size > anim->streamSize)return false;

  const uint32_t halfSize = anim->bufferHalfSize;
  uint32_t pos = anim->readPos;
  uint32_t halfEnd = pos >= halfSize ? (halfSize * 2) : halfSize;
  const uint16_t *src = (const uint16_t*)(anim->buffer + pos);
  uint16_t *dst = (uint16_t*)kf;

  if(pos + 8 <= halfEnd) { // always copy 8 bytes, the check makes sure it never touches the other half
    *(u64_alias_t*)kf = *(const u64_unaligned_t*)src;
  } else { // crosses into the other half, which may wrap around to the start of the buffer
    uint32_t sizeA = (halfEnd - pos) / 2;
    for(uint32_t i=0; i<sizeA; ++i)dst[i] = src[i];
    stream_wait(anim);
    src = (const uint16_t*)(anim->buffer + (halfEnd == halfSize ? halfSize : 0));
    for(uint32_t i=sizeA; i<size/2; ++i)dst[i] = src[i - sizeA];
  }

  pos += size;
  anim->streamPos += size;
  if(pos >= halfEnd) { // current half fully read, switch and refill it
    stream_wait(anim);
    if(pos >= halfSize * 2)pos -= halfSize * 2;
    stream_load(anim, halfEnd == halfSize ? 0 : 1);
  }
  anim->readPos = pos;
  return true;
}

T3DAnim t3d_anim_create_buffered(const T3DModel *model, const char *name, uint32_t bufferSize) {
  T3DChunkAnim* animDef = t3d_model_get_animation(model, name);
  assertf(animDef, "Animation '%s' not found in model", name);
  assertf(bufferSize >= 32 && (bufferSize % 32) == 0 && bufferSize <= 0x8000, "Invalid animation buffer size: %lu", bufferSize);

  const char *path = animDef->filePath;
  path += 5;
  pi_addr_t romAddr = dfs_rom_addr(path);
  int streamSize = dfs_rom_size(path);
  assertf(romAddr != 0 && streamSize >= 0, "Animation data not found: %s", animDef->filePath);
  assertf((romAddr & 1) == 0 && (streamSize & 1) == 0, "Animation data not 2-byte aligned: %s", animDef->filePath);

  T3DAnim anim = {
    .animRef = animDef,
    .targetsQuat = NULL,
    .targetsScalar = NULL,
    .speed = 1.0f,
    .time = 0.0f,
    .buffer = memalign(16, bufferSize), // own cache-lines, needed for the invalidate before each DMA
    .dmaTicket = 0,
    .romAddr = romAddr,
    .streamSize = streamSize,
    .bufferHalfSize = bufferSize / 2,
    .nextKfSize = sizeof(T3DAnimKF),
    .isPlaying = 1,
    .isLooping = 1
  };
  // DMAs target the heap buffer, so returning the struct by value is fine
  stream_reset(&anim);
  return anim;
}

static void rewind_anim(T3DAnim *anim)
{
  for(int c=0; c<anim->animRef->channelsScalar; c++) {
    anim->targetsScalar[c].base.timeEnd = 0;
  }
  for(int c=0; c<anim->animRef->channelsQuat; c++) {
    anim->targetsQuat[c].base.timeEnd = 0;
  }
  anim->nextKfSize = sizeof(T3DAnimKF);
  stream_rewind(anim);
}

static inline bool load_keyframe(T3DAnim *anim);
void t3d_anim_attach(T3DAnim *anim, const T3DSkeleton *skeleton) {
  if(anim->targetsQuat)free(anim->targetsQuat);

  size_t allocQuat = sizeof(T3DAnimTargetQuat) * anim->animRef->channelsQuat;
  size_t allocScalar = sizeof(T3DAnimTargetScalar) * anim->animRef->channelsScalar;
  anim->targetsQuat = calloc(allocQuat + allocScalar, 1); // only allocate a single block
  anim->targetsScalar = (T3DAnimTargetScalar*)((uint8_t*)anim->targetsQuat + allocQuat);
  rewind_anim(anim);

  uint32_t channelCount = anim->animRef->channelsScalar + anim->animRef->channelsQuat;

  uint32_t idxQuat = 0;
  uint32_t idxScalar = 0;
  for(uint32_t i = 0; i < channelCount; i++)
  {
    T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[i];
    T3DBone *bone = &skeleton->bones[channelMap->targetIdx];

    switch(channelMap->targetType) {
      case T3D_ANIM_TARGET_TRANSLATION:
        anim->targetsScalar[idxScalar].targetScalar = &bone->position.v[channelMap->attributeIdx];
        anim->targetsScalar[idxScalar++].base.changedFlag = &bone->hasChanged;
        break;
      case T3D_ANIM_TARGET_SCALE_XYZ:
        anim->targetsScalar[idxScalar].targetScalar = &bone->scale.v[channelMap->attributeIdx];
        anim->targetsScalar[idxScalar++].base.changedFlag = &bone->hasChanged;
        break;
      case T3D_ANIM_TARGET_ROTATION:
        anim->targetsQuat[idxQuat].targetQuat = &bone->rotation;
        anim->targetsQuat[idxQuat++].base.changedFlag = &bone->hasChanged;
      break;
      default: {assertf(false, "Unknown animation target %d", channelMap->targetType);}
    }
  }
  
  uint32_t initCount = 0;
  while(initCount < channelCount) {
    if(!load_keyframe(anim)) break;
    initCount = 0;
    for(int c = 0; c < anim->animRef->channelsQuat; c++) {
      if(anim->targetsQuat[c].base.timeEnd > 0) initCount++;
    }
    for(int c = 0; c < anim->animRef->channelsScalar; c++) {
      if(anim->targetsScalar[c].base.timeEnd > 0) initCount++;
    }
  }
  for(int c = 0; c < anim->animRef->channelsQuat; c++) {
    anim->targetsQuat[c].kfCurr = anim->targetsQuat[c].kfNext;
  }
  for(int c = 0; c < anim->animRef->channelsScalar; c++) {
    anim->targetsScalar[c].kfCurr = anim->targetsScalar[c].kfNext;
  }
  rewind_anim(anim);
}

inline static void attach_scalar(T3DAnim* anim, uint32_t targetIdx, T3DVec3* target, int32_t *updateFlag, uint8_t targetType) {
  for(int i = 0; i < anim->animRef->channelsScalar; i++) {
    T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[i+anim->animRef->channelsQuat];
    if(channelMap->targetIdx == targetIdx && channelMap->targetType == targetType) {
      anim->targetsScalar[i].targetScalar = &target->v[channelMap->attributeIdx];
      anim->targetsScalar[i].base.changedFlag = updateFlag;
    }
  }
}

void t3d_anim_attach_pos(T3DAnim* anim, uint32_t targetIdx, T3DVec3* target, int32_t *updateFlag) {
  attach_scalar(anim, targetIdx, target, updateFlag, T3D_ANIM_TARGET_TRANSLATION);
}

void t3d_anim_attach_scale(T3DAnim *anim, uint32_t targetIdx, T3DVec3 *target, int32_t *updateFlag) {
  attach_scalar(anim, targetIdx, target, updateFlag, T3D_ANIM_TARGET_SCALE_XYZ);
}

void t3d_anim_attach_rot(T3DAnim *anim, uint32_t targetIdx, T3DQuat *target, int32_t *updateFlag) {
  for(int i = 0; i < anim->animRef->channelsQuat; i++) {
    T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[i];
    if(channelMap->targetIdx == targetIdx && channelMap->targetType == T3D_ANIM_TARGET_ROTATION) {
      anim->targetsQuat[i].targetQuat = target;
      anim->targetsQuat[i].base.changedFlag = updateFlag;
    }
  }
}

static inline float s10ToFloat(uint32_t value, float offset, float scale) {
  return (float)value / 1023.0f * scale + offset;
}

static inline void unpack_quat(uint16_t dataHi, uint16_t dataLo, T3DQuat *out) {
  int largestIdx = dataHi >> 14;
  int idx0 = (largestIdx + 1) & 0b11;
  int idx1 = (largestIdx + 2) & 0b11;
  int idx2 = (largestIdx + 3) & 0b11;

  uint16_t dataMid = (dataHi << 6) | (dataLo >> 10);
  float q0 = s10ToFloat((dataHi >> 4) & 0x3FF, -SQRT_2_INV, SQRT_2_INV+SQRT_2_INV);
  float q1 = s10ToFloat((dataMid    ) & 0x3FF, -SQRT_2_INV, SQRT_2_INV+SQRT_2_INV);
  float q2 = s10ToFloat((dataLo     ) & 0x3FF, -SQRT_2_INV, SQRT_2_INV+SQRT_2_INV);

  out->v[idx0] = q0;
  out->v[idx1] = q1;
  out->v[idx2] = q2;
  out->v[largestIdx] = sqrtf(1.0f - q0*q0 - q1*q1 - q2*q2);
}

static inline T3DAnimTargetBase* get_base_target(T3DAnim *anim, uint64_t channelIdx, bool isRot) {
  return isRot ?
    (T3DAnimTargetBase*)&anim->targetsQuat[channelIdx] :
    (T3DAnimTargetBase*)&anim->targetsScalar[channelIdx - anim->animRef->channelsQuat];
}

static inline bool load_keyframe(T3DAnim *anim) {
  T3DAnimKF kf __attribute__((aligned(8), uninitialized)); // 8-byte aligned for the 64-bit copy
  if(!stream_read_kf(anim, &kf))return false;

  bool isLarge = kf.nextTime & 0x8000;
  anim->nextKfSize = isLarge ? sizeof(T3DAnimKF) : (sizeof(T3DAnimKF)-2);
  kf.nextTime &= 0x7FFF;

  T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[kf.channelIdx];

  bool isRot = kf.channelIdx < anim->animRef->channelsQuat;
  T3DAnimTargetBase *targetBase = get_base_target(anim, kf.channelIdx, isRot);

  targetBase->timeStart = targetBase->timeEnd;
  targetBase->timeEnd += (float)kf.nextTime * KF_TIME_TICK;
  if(kf.nextTime == 0)targetBase->timeStart -= 0.00001f; // avoid zero-div for overlapping keyframes

  if(channelMap->targetType == T3D_ANIM_TARGET_ROTATION) {
    T3DAnimTargetQuat *target = (T3DAnimTargetQuat*)targetBase;
    // 64-bit copy, otherwise we get a memcpy
    ((u64_alias_t*)&target->kfCurr)[0] = ((u64_alias_t*)&target->kfNext)[0];
    ((u64_alias_t*)&target->kfCurr)[1] = ((u64_alias_t*)&target->kfNext)[1];
    unpack_quat(kf.data[0], kf.data[1], &target->kfNext);
  } else {
    T3DAnimTargetScalar *target = (T3DAnimTargetScalar*)targetBase;
    target->kfCurr = target->kfNext;
    target->kfNext = (float)kf.data[0] * channelMap->quantScale + channelMap->quantOffset;
  }

  return true;
}

// Local copy for better cache usage
static inline void local_quat_nlerp(T3DQuat *res, const T3DQuat *a, const T3DQuat *b, float t) {
  float blend = 1.0f - t;
  if(t3d_quat_dot(a, b) < 0.0f) {
    blend = -blend;
  }
  res->v[0] = blend * a->v[0] + t * b->v[0];
  res->v[1] = blend * a->v[1] + t * b->v[1];
  res->v[2] = blend * a->v[2] + t * b->v[2];
  res->v[3] = blend * a->v[3] + t * b->v[3];
  t3d_quat_normalize(res);
}

void t3d_anim_update(T3DAnim *anim, float deltaTime) {
  if(!anim->isPlaying)return;
  int32_t updateFlag = 1;
  anim->time += deltaTime * anim->speed;

  if(anim->time >= anim->animRef->duration) {
    anim->time -= anim->animRef->duration;
    rewind_anim(anim);
    updateFlag = 2;

    if(!anim->isLooping) {
      anim->isPlaying = 0;
      return;
    }
  }

  // local copies, stores through the target pointers below could alias 'anim' and force reloads otherwise
  const float time = anim->time;
  const uint32_t channelsQuat = anim->animRef->channelsQuat;
  const uint32_t channelCount = anim->animRef->channelsScalar + channelsQuat;
  T3DAnimTargetQuat *targetsQuat = anim->targetsQuat;
  T3DAnimTargetScalar *targetsScalar = anim->targetsScalar;

  for(uint32_t c=0; c<channelCount; c++)
  {
    bool isRot = c < channelsQuat;
    T3DAnimTargetBase *target = isRot ?
      (T3DAnimTargetBase*)&targetsQuat[c] :
      (T3DAnimTargetBase*)&targetsScalar[c - channelsQuat];

    while(time >= target->timeEnd) {
      if(!load_keyframe(anim))break;
    }

    float timeDiff = target->timeEnd - target->timeStart;
    float interp = (time - target->timeStart) / timeDiff;
    *target->changedFlag = updateFlag;

    if(isRot) {
      T3DAnimTargetQuat *t = (T3DAnimTargetQuat*)target;
      local_quat_nlerp(t->targetQuat, &t->kfCurr, &t->kfNext, interp);
    } else {
      T3DAnimTargetScalar *t = (T3DAnimTargetScalar*)target;
      *t->targetScalar = t3d_lerp(t->kfCurr, t->kfNext, interp);
    }
  }
}

void t3d_anim_destroy(T3DAnim *anim) {
  if(anim->targetsQuat)free(anim->targetsQuat); // 'targetsScalar' is part of this memory-block
  if(anim->buffer) {
    stream_wait(anim); // DMAs could still be writing into the buffer
    free(anim->buffer);
  }
  anim->targetsQuat = NULL;
  anim->targetsScalar = NULL;
  anim->buffer = NULL;
}

void t3d_anim_set_time(T3DAnim *anim, float time) {
  if(time > anim->animRef->duration)time = anim->animRef->duration;
  if(time < anim->time)rewind_anim(anim);
  anim->time = time;
}
