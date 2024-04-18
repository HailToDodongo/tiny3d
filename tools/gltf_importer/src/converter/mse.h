/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include "../structs.h"
#include "../math/sampling.h"

inline float calcMSE(const std::vector<Keyframe> &keyframes, const std::vector<Keyframe> &keyframesOrg, float duration, u32 channelIdx) {
  float sampleRate = 60.0f;
  float timeStep = 1.0f / sampleRate;

  int sampleCount = 0;
  float mse = 0.0f;

  for(float t=0; t<duration; t += timeStep)
  {
    float tPage = t;
    auto idxOrg = (uint32_t)(t * sampleRate);
    float interpOrg = (tPage * sampleRate) - (float)idxOrg;

    auto idxNew = (uint32_t)(t * sampleRate);
    float interpNew = (t * sampleRate) - (float)idxNew;

    //printf("idx: %d -> %d, interp: %f / %f\n", idxOrg, idxNew, interpOrg, interpNew);

    /*if(channelOrg.isRotation()) {
      auto quatOrg = interpolQuat(channelOrg.valQuat, idxOrg, interpOrg).toVec4();
      auto quatNew = interpolQuat(channelNew.valQuat, idxNew, interpNew).toVec4();
      mse += (quatOrg - quatNew).length2();
    } else {
      float diff = interpolScalar(channelOrg.valScalar, idxOrg, interpOrg)
                 - interpolScalar(channelNew.valScalar, idxNew, interpNew);
      mse += diff * diff;
    }*/
    ++sampleCount;
  }

  return mse / (float)sampleCount;
}