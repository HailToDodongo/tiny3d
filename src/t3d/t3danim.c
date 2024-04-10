/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "t3d/t3danim.h"
#include <malloc.h>

#define SQRT_2_INV 0.70710678118f
const static char TARGET_TYPE[4] = {'T', 'S', 's', 'R'};

T3DAnim t3d_anim_create(const T3DModel *model, const char *name) {
  T3DChunkAnim* animDef = t3d_model_get_animation(model, name);
  assertf(animDef, "Animation '%s' not found in model", name);

  T3DAnim result = {
    .animRef = animDef,
    .targets = NULL,
    .time = 0.0f,
    .speed = 1.0f,
    .timeNextPage = 0,
    .loadedPageIdx = -1,
    .pageData = memalign(16, animDef->maxPageSize),
  };
  return result;
}

void t3d_anim_attach(T3DAnim *anim, const T3DSkeleton *skeleton) {
  debugf("\n\n\n\n");
  debugf("Attaching animation '%s' to skeleton\n", anim->animRef->name);
  anim->targets = malloc(sizeof(T3DAnimTarget) * anim->animRef->channelCount);
  for(int i = 0; i < anim->animRef->channelCount; i++)
  {
    T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[i];
    T3DAnimTarget *target = &anim->targets[i];
    T3DBone *bone = &skeleton->bones[channelMap->targetIdx];

    debugf("  - Channel %d: type: %d\n", i, channelMap->targetType);

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

  debugf("\n\n\n\n");
  // targets
}

static inline void load_page(T3DAnim *anim, int index) {
  debugf("Loading page %d\n", index);
  const T3DAnimPage *page = &anim->animRef->pageTable[index];
  data_cache_hit_writeback_invalidate(anim->pageData, page->dataSize);
  dma_read(anim->pageData, anim->animRef->sdataAddrROM + page->dataOffset, page->dataSize);
  anim->loadedPageIdx = index;
}

static inline float s10ToFloat(uint32_t value, float offset, float scale) {
  return (float)value / 1023.0f * scale + offset;
}

static inline void unpack_quat(uint32_t data, T3DQuat *out) {
  int largestIdx = data >> 30;

  int idx0 = (largestIdx + 1) % 4;
  int idx1 = (largestIdx + 2) % 4;
  int idx2 = (largestIdx + 3) % 4;

  float q0 = s10ToFloat((data >> 20) & 0x3FF, -SQRT_2_INV, SQRT_2_INV+SQRT_2_INV);
  float q1 = s10ToFloat((data >> 10) & 0x3FF, -SQRT_2_INV, SQRT_2_INV+SQRT_2_INV);
  float q2 = s10ToFloat((data      ) & 0x3FF, -SQRT_2_INV, SQRT_2_INV+SQRT_2_INV);

  out->v[idx0] = q0;
  out->v[idx1] = q1;
  out->v[idx2] = q2;
  out->v[largestIdx] = sqrtf(1.0f - q0*q0 - q1*q1 - q2*q2);
}

void t3d_anim_update(T3DAnim *anim, float deltaTime) {
  if(anim->loadedPageIdx < 0) {
    load_page(anim, 0);
  }

  anim->time += deltaTime * anim->speed;

  if(anim->time >= anim->animRef->duration) {
    anim->time -= anim->animRef->duration;
    anim->loadedPageIdx = 0;
  }

  const T3DAnimPage *page = &anim->animRef->pageTable[anim->loadedPageIdx];
  float kfIdxFloat = anim->time * page->sampleRate;
  uint32_t kfIdx = (uint32_t)kfIdxFloat;
  float interp = kfIdxFloat - kfIdx;

  debugf("Time: %.2f, KF: %ld\n", anim->time, kfIdx);

  const char *data = anim->pageData;
  for(int c=0; c<anim->animRef->channelCount; c++)
  {
    //debugf("  - Channel %d 0x%08lX:", c, (uint32_t)data);
    T3DAnimChannelMapping *channelMap = &anim->animRef->channelMappings[c];
    T3DAnimTarget *target = &anim->targets[c];

    if(channelMap->targetType == T3D_ANIM_TARGET_ROTATION) {
      uint32_t* dataU32 = (uint32_t*)(data + (kfIdx*4));
      T3DQuat quatNext;
      unpack_quat(dataU32[0], (T3DQuat*)target->target);
      unpack_quat(dataU32[1], &quatNext);
      t3d_quat_nlerp((T3DQuat*)target->target, (T3DQuat*)target->target, &quatNext, interp);

      /*debugf(" %08lX @ %d -> %.2f %.2f %.2f %.2f (stride: %d)\n",
        *dataU32, data - anim->pageData,
        ((T3DQuat*)target->target)->v[0],
        ((T3DQuat*)target->target)->v[1],
        ((T3DQuat*)target->target)->v[2],
        ((T3DQuat*)target->target)->v[3],
        page->strideWords
      );*/
      data += page->strideWords * 8;
    } else {
      if(channelMap->targetType == T3D_ANIM_TARGET_SCALE_XYZ) { // @TODO: normalize pos in gltf importer
        uint16_t* dataU16 = (uint16_t*)(data + (kfIdx*2));
        float valueA = (float)dataU16[0] * channelMap->quantScale + channelMap->quantOffset;
        float valueB = (float)dataU16[1] * channelMap->quantScale + channelMap->quantOffset; // @TODO: OOB

        *(float*)target->target = valueA + (valueB - valueA) * interp;
        /*debugf(" (%c) %04X @ %d -> %.2f (stride: %d)\n", TARGET_TYPE[channelMap->targetType],
        *dataU16, data - anim->pageData, *(float*)target->target, page->strideWords);*/
      }

      data += page->strideWords * 4;
    }
    *target->changedFlag = 1;
  }
}

void t3d_anim_destroy(T3DAnim *anim) {
  if(anim->targets)free(anim->targets);
  anim->targets = NULL;
}

