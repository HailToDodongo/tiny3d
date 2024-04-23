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
    .targetsScalar = NULL,
    .targetsQuat = NULL,
    .time = 0.0f,
    .speed = 1.0f,
    .nextKfSize = sizeof(T3DAnimKF),
    .kfIndex = 0,
    .file = asset_fopen(animDef->filePath, NULL),
  };
}

void t3d_anim_attach(T3DAnim *anim, const T3DSkeleton *skeleton) {
  if(anim->targetsQuat)free(anim->targetsQuat);
  if(anim->targetsScalar)free(anim->targetsScalar);

  anim->targetsQuat = malloc(sizeof(T3DAnimTargetQuat) * anim->animRef->channelsQuat);
  anim->targetsScalar = malloc(sizeof(T3DAnimTargetScalar) * anim->animRef->channelsScalar);
  uint32_t channelCount = anim->animRef->channelsScalar + anim->animRef->channelsQuat;

  uint32_t idxQuat = 0;
  uint32_t idxScalar = 0;
  for(int i = 0; i < channelCount; i++)
  {
    T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[i];
    T3DBone *bone = &skeleton->bones[channelMap->targetIdx];

    switch(channelMap->targetType) {
      case T3D_ANIM_TARGET_TRANSLATION:
        anim->targetsScalar[idxScalar].targetScalar = &bone->position.v[channelMap->attributeIdx];
        anim->targetsScalar[idxScalar].base.changedFlag = &bone->hasChanged;
        ++idxScalar;
        break;
      case T3D_ANIM_TARGET_SCALE_XYZ:
        anim->targetsScalar[idxScalar].targetScalar = &bone->scale.v[channelMap->attributeIdx];
        anim->targetsScalar[idxScalar].base.changedFlag = &bone->hasChanged;
        ++idxScalar;
        break;
      case T3D_ANIM_TARGET_ROTATION:
        anim->targetsQuat[idxQuat].targetQuat = &bone->rotation;
        anim->targetsQuat[idxQuat].base.changedFlag = &bone->hasChanged;
        ++idxQuat;
      break;
      default: {
        assertf(false, "Unknown animation target %d", channelMap->targetType);
      }
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
  T3DAnimKF kf;
  size_t readBytes = fread(&kf, anim->nextKfSize, 1, anim->file);
  if(readBytes == 0)return false;

  // KF Header
  bool isLarge = kf.nextTime & 0x8000;
  anim->nextKfSize = isLarge ? sizeof(T3DAnimKF) : (sizeof(T3DAnimKF)-2);
  kf.nextTime &= 0x7FFF;

  // KF Data
  T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[kf.channelIdx];

  bool isRot = kf.channelIdx < anim->animRef->channelsQuat;
  T3DAnimTargetBase *targetBase = get_base_target(anim, kf.channelIdx, isRot);

  targetBase->timeNextKF += (float)kf.nextTime * KF_TIME_TICK;
  targetBase->timeStart = targetBase->timeEnd;
  targetBase->timeEnd = targetBase->timeNextKF - channelMap->timeOffset;

  //debugf("Load KF[%d], %.4f - %.4f (%.4f, +%d)\n", anim->kfIndex, 0.0f, 0.0f, 0.0f, kf.nextTime);

  if(channelMap->targetType == T3D_ANIM_TARGET_ROTATION) {
    T3DAnimTargetQuat *target = (T3DAnimTargetQuat*)targetBase;
    target->kfCurr = target->kfNext;
    unpack_quat(kf.data[0], kf.data[1], &target->kfNext);

  } else {
    T3DAnimTargetScalar *target = (T3DAnimTargetScalar*)targetBase;
    target->kfCurr = target->kfNext;
    target->kfNext = (float)kf.data[0] * channelMap->quantScale + channelMap->quantOffset;
  }

  ++anim->kfIndex;
  return true;
}

void t3d_anim_update(T3DAnim *anim, float deltaTime) {
  anim->time += deltaTime * anim->speed;

  if(anim->time >= anim->animRef->duration)
  {
    anim->time -= anim->animRef->duration;
    for(int c=0; c<anim->animRef->channelsScalar; c++) {
      anim->targetsScalar[c].base.timeNextKF = 0;
    }
    for(int c=0; c<anim->animRef->channelsQuat; c++) {
      anim->targetsQuat[c].base.timeNextKF = 0;
    }
    anim->nextKfSize = sizeof(T3DAnimKF);
    anim->kfIndex = 0;
    rewind(anim->file);
    debugf("Looping animation\n");
  }

  uint32_t channelCount = anim->animRef->channelsScalar + anim->animRef->channelsQuat;
  for(int c=0; c<channelCount; c++)
  {
    bool isRot = c < anim->animRef->channelsQuat;
    T3DAnimTargetBase *target = get_base_target(anim, c, isRot);

    while(anim->time >= target->timeNextKF) {
      if(!load_keyframe(anim))break;
    }
    //debugf("Time: %.2f (%.2f -> %.2f)\n", anim->time, target->timeStart, target->timeEnd);

    float timeDiff = target->timeEnd - target->timeStart;
    float interp = (anim->time - target->timeStart) / timeDiff;
    *target->changedFlag = 1;

    if(isRot) {
      T3DAnimTargetQuat *t = (T3DAnimTargetQuat*)target;
      t3d_quat_nlerp(t->targetQuat, &t->kfCurr, &t->kfNext, interp);
      //t3d_quat_slerp(target->target, &target->kfCurr, &target->kfNext, interp);
    } else {
      T3DAnimTargetScalar *t = (T3DAnimTargetScalar*)target;
      *t->targetScalar = t3d_lerp(t->kfCurr, t->kfNext, interp);
    }
  }
}

void t3d_anim_destroy(T3DAnim *anim) {
  if(anim->targetsScalar)free(anim->targetsScalar);
  if(anim->targetsQuat)free(anim->targetsQuat);
  anim->targetsScalar = NULL;
  anim->targetsQuat = NULL;
  // TODO: close file
}

void t3d_anim_set_time(T3DAnim *anim, float time) {
  assert(false); // @TODO: implement
}

