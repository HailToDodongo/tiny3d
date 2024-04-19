/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "t3d/t3danim.h"
#include <malloc.h>

#define SQRT_2_INV 0.70710678118f
#define KF_TIME_TICK (1.0f / 60.0f)

const static char TARGET_TYPE[4] = {'T', 'S', 's', 'R'};

typedef struct {
  uint16_t nextTime;
  uint16_t channelIdx;
  uint16_t data[2];
} T3DAnimKF;

T3DAnim t3d_anim_create(const T3DModel *model, const char *name) {
  T3DChunkAnim* animDef = t3d_model_get_animation(model, name);
  assertf(animDef, "Animation '%s' not found in model", name);

  return (T3DAnim){
    .animRef = animDef,
    .targets = NULL,
    .time = 0.0f,
    .speed = 1.0f,
    .timeNextKF = 0,
    .nextKfSize = sizeof(T3DAnimKF),
    .file = asset_fopen(animDef->filePath, NULL),
    //.pageData = memalign(16, animDef->maxPageSize),
  };
}

void t3d_anim_attach(T3DAnim *anim, const T3DSkeleton *skeleton) {
  if(anim->targets)free(anim->targets);
  anim->targets = malloc(sizeof(T3DAnimTarget) * anim->animRef->channelCount);
  for(int i = 0; i < anim->animRef->channelCount; i++)
  {
    T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[i];
    T3DAnimTarget *target = &anim->targets[i];
    T3DBone *bone = &skeleton->bones[channelMap->targetIdx];

    target->changedFlag = &bone->hasChanged;
    switch(channelMap->targetType) {
      case T3D_ANIM_TARGET_TRANSLATION:
        target->target = &bone->position.v[channelMap->attributeIdx];
        break;
      case T3D_ANIM_TARGET_SCALE_XYZ:
        target->target = &bone->scale.v[channelMap->attributeIdx];
        break;
      case T3D_ANIM_TARGET_SCALE_S: target->target = &bone->scale; break;
      case T3D_ANIM_TARGET_ROTATION: target->target = &bone->rotation; break;
      default: {
        assertf(false, "Unknown animation target %d", channelMap->targetType);
      }
    }
  }
}

static inline void load_keyframe(T3DAnim *anim, int index) {
  /*bool isLastPage = index == anim->animRef->pageCount - 1;
  const T3DAnimPage *page = &anim->animRef->pageTable[index];
  const T3DAnimPage *nextPage = &anim->animRef->pageTable[isLastPage ? 0 : (index+1)];
  uint32_t dataSize = isLastPage ? anim->animRef->maxPageSize : (nextPage->dataOffset - page->dataOffset);

  data_cache_hit_writeback_invalidate(anim->pageData, dataSize);
  dma_read(anim->pageData, anim->animRef->sdataAddrROM + page->dataOffset, dataSize);
  anim->loadedPageIdx = index;

  if(!isLastPage) {
    anim->timeNextPage = nextPage->timeStart;
  } else {
    anim->timeNextPage = anim->animRef->duration;
  }*/
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

static inline void load_kf(T3DAnim *anim) {
  T3DAnimKF kf;
  fread(&kf, anim->nextKfSize, 1, anim->file);

  // KF Header
  bool isLarge = kf.nextTime & 0x8000;
  anim->nextKfSize = isLarge ? sizeof(T3DAnimKF) : (sizeof(T3DAnimKF)-2);
  kf.nextTime &= 0x7FFF;
  anim->timeNextKF += (float)kf.nextTime * KF_TIME_TICK;

  // KF Data
  T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[kf.channelIdx];
  T3DAnimTarget *target = &anim->targets[kf.channelIdx];

  if(channelMap->targetType == T3D_ANIM_TARGET_ROTATION) {
    T3DQuat quat;
    debugf("KF | ch: %d, next: %.4fs -> (%04X %04X) ", kf.channelIdx, anim->timeNextKF, kf.data[0], kf.data[1]);
    unpack_quat(kf.data[0], kf.data[1], &quat);
    debugf("%.4f %.4f %.4f %.4f\n", quat.v[0], quat.v[1], quat.v[2], quat.v[3]);
  } else {
    float value = (float)kf.data[0] * channelMap->quantScale + channelMap->quantOffset;
    debugf("KF | ch: %d, next: %.4fs -> %.4f\n", kf.channelIdx, anim->timeNextKF, value);
  }
}

void t3d_anim_update(T3DAnim *anim, float deltaTime) {
  anim->time += deltaTime * anim->speed;

  if(anim->time >= anim->animRef->duration) {
    anim->time -= anim->animRef->duration;
    anim->timeNextKF = anim->time;
    anim->nextKfSize = sizeof(T3DAnimKF);

    // rewind(anim->file); // <- @TODO: use when libdragon allows for it
    fclose(anim->file);
    anim->file = asset_fopen(anim->animRef->filePath, NULL);

    debugf("Looping animation\n");
  }

  while(anim->time >= anim->timeNextKF) {
    load_kf(anim);
  }

  debugf("Time: %.2f\n", anim->time);
  return;

  const char *data = anim->pageData;
  for(int c=0; c<anim->animRef->channelCount; c++)
  {
    //debugf("  - Channel %d 0x%08lX:", c, (uint32_t)data);
    T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[c];
    T3DAnimTarget *target = &anim->targets[c];

    uint8_t sampleRate = data[0];
    uint8_t sampleCount = data[1];
    data += 2;

    float kfIdxFloat = 0.0f;//(anim->time - page->timeStart) * sampleRate;
    uint32_t kfIdx = (uint32_t)kfIdxFloat;
    uint32_t kfIdxNext = kfIdx + 1;
    float interp = kfIdxFloat - kfIdx;

    if(kfIdx >= sampleCount)kfIdx = sampleCount-1;
    if(kfIdxNext >= sampleCount)kfIdxNext = sampleCount-1;

    if(channelMap->targetType == T3D_ANIM_TARGET_ROTATION) {
      uint16_t* dataU16 = (uint16_t*)(data + (kfIdx*4));
      uint16_t* dataU16Next = (uint16_t*)(data + (kfIdxNext*4));
      T3DQuat quatNext;
      unpack_quat(dataU16[0], dataU16[1], (T3DQuat*)target->target);
      unpack_quat(dataU16Next[0], dataU16Next[1], &quatNext);
      t3d_quat_nlerp((T3DQuat*)target->target, (T3DQuat*)target->target, &quatNext, interp);
      //t3d_quat_slerp((T3DQuat*)target->target, (T3DQuat*)target->target, &quatNext, interp);

      /*debugf(" %08lX @ %d -> %.2f %.2f %.2f %.2f (stride: %d)\n",
        *dataU32, data - anim->pageData,
        ((T3DQuat*)target->target)->v[0],
        ((T3DQuat*)target->target)->v[1],
        ((T3DQuat*)target->target)->v[2],
        ((T3DQuat*)target->target)->v[3],
        page->strideWords
      );*/
      data += sampleCount * 4;
    } else {
      uint16_t dataU16 = *(uint16_t*)(data + (kfIdx*2));
      uint16_t dataU16Next = *(uint16_t*)(data + (kfIdxNext*2));
      float valueA = (float)dataU16 * channelMap->quantScale + channelMap->quantOffset;
      float valueB = (float)dataU16Next * channelMap->quantScale + channelMap->quantOffset; // @TODO: OOB

      *(float*)target->target = valueA + (valueB - valueA) * interp;
      //debugf(" (%c) %04X @ %d -> %.2f (stride: %d)\n", TARGET_TYPE[channelMap->targetType],
      //*dataU16, data - anim->pageData, *(float*)target->target, page->strideWords);

      data += sampleCount * 2;
    }
    *target->changedFlag = 1;
  }
}

void t3d_anim_destroy(T3DAnim *anim) {
  if(anim->targets)free(anim->targets);
  anim->targets = NULL;
  if(anim->pageData)free(anim->pageData);
  anim->pageData = NULL;
}

void t3d_anim_set_time(T3DAnim *anim, float time) {
  assert(false); // @TODO: implement
}

