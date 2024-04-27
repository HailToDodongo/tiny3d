/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include "../structs.h"

inline const Keyframe& safeKf(const std::vector<Keyframe> &kfs, int idx) {
  if(idx < 0)return kfs[0];
  if(idx >= kfs.size())return kfs.back();
  return kfs[idx];
}

inline float calcMSE(const std::vector<Keyframe> &kfsNew, const std::vector<Keyframe> &kfsOrg, float timeStart, float timeEnd, bool isRotation) {
  float sampleRate = 60.0f;
  float timeStep = 1.0f / sampleRate;

  int sampleCount = 0;
  float mse = 0.0f;

  int idxNew = 0;
  int idxOrg = 0;

  for(float t=timeStart; t<timeEnd; t += timeStep)
  {
    while(t >= safeKf(kfsNew, idxNew+1).time) {
      ++idxNew; if(idxNew >= kfsNew.size())break;
    }
    while(t >= safeKf(kfsOrg, idxOrg+1).time) {
      ++idxOrg; if(idxOrg >= kfsOrg.size())break;
    }

    const Keyframe &kfOrg = safeKf(kfsOrg, idxOrg);
    const Keyframe &kfOrgNext = safeKf(kfsOrg, idxOrg + 1);
    const Keyframe &kfNew = safeKf(kfsNew, idxNew);
    const Keyframe &kfNewNext = safeKf(kfsNew, idxNew + 1);

    float tDiffOrg = kfOrgNext.time - kfOrg.time;
    float tDiffNew = kfNewNext.time - kfNew.time;

    float interpOrg = (tDiffOrg > 0.00001f) ? ((t - kfOrg.time) / tDiffOrg) : 0.0f;
    float interpNew = (tDiffNew > 0.00001f) ? ((t - kfNew.time) / tDiffNew) : 0.0f;

    if(isRotation) {
      Vec4 quatOrg = kfOrg.valQuat.slerp(kfOrgNext.valQuat, interpOrg).toVec4();
      Vec4 quatNew = kfNew.valQuat.slerp(kfNewNext.valQuat, interpNew).toVec4();
      mse += (quatOrg - quatNew).length2();
    } else {
      float valOrg = kfOrg.valScalar + (kfOrgNext.valScalar - kfOrg.valScalar) * interpOrg;
      float valNew = kfNew.valScalar + (kfNewNext.valScalar - kfNew.valScalar) * interpNew;
      mse += (valOrg - valNew) * (valOrg - valNew);
    }
    ++sampleCount;
  }
  return mse / (float)sampleCount;
}