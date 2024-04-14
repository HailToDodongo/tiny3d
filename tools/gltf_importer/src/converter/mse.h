/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include "../structs.h"
#include "../math/sampling.h"

inline float calcMSE(const AnimPage &originalPage, const std::vector<AnimPage> &newPages, u32 channelIdx) {
  float timeStep = 1.0f / originalPage.sampleRate;

  int sampleCount = 0;
  float mse = 0.0f;
  for(auto &page : newPages)
  {
    const auto &channelOrg = originalPage.channels[channelIdx];
    const auto &channelNew = page.channels[channelIdx];

    for(float t=0; t<page.duration; t += timeStep)
    {
      float tPage = t + page.timeStart;
      auto idxOrg = (uint32_t)(tPage * originalPage.sampleRate);
      float interpOrg = (tPage * originalPage.sampleRate) - (float)idxOrg;

      auto idxNew = (uint32_t)(t * page.sampleRate);
      float interpNew = (t * page.sampleRate) - (float)idxNew;

      //printf("idx: %d -> %d, interp: %f / %f\n", idxOrg, idxNew, interpOrg, interpNew);

      if(channelOrg.isRotation()) {
        auto quatOrg = interpolQuat(channelOrg.valQuat, idxOrg, interpOrg).toVec4();
        auto quatNew = interpolQuat(channelNew.valQuat, idxNew, interpNew).toVec4();
        mse += (quatOrg - quatNew).length2();
      } else {
        float diff = interpolScalar(channelOrg.valScalar, idxOrg, interpOrg)
                   - interpolScalar(channelNew.valScalar, idxNew, interpNew);

         if(fabs(diff) > 0.0001f) {
          printf("   : %.4f -> %.4f (diff: %f) ====================================\n", interpolScalar(channelOrg.valScalar, idxOrg, interpOrg), interpolScalar(channelNew.valScalar, idxNew, interpNew), diff);
         } else {
          //( printf("   : %.4f -> %.4f\n", interpolScalar(channelOrg.valScalar, idxOrg, interpOrg), interpolScalar(channelNew.valScalar, idxNew, interpNew));
         };
        mse += diff * diff;
      }
      ++sampleCount;
    }
  }
  return mse / (float)sampleCount;
}