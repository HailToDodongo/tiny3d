/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once

#include "quat.h"
#include <vector>
#include <cstdint>

inline Quat interpolQuat(const std::vector<Quat> &quats, uint32_t idx, float interp) {
  if(idx >= quats.size())return quats.back();
  int idxNext = idx + 1;
  if(idxNext >= quats.size())return quats.back();

  Quat q1 = quats[idx];
  Quat q2 = quats[idxNext];
  if(interp < 0.0001f)return q1;
  if(interp > 0.9999f)return q2;
  return q1.slerp(q2, interp);
}

inline float interpolScalar(const std::vector<float> &scalars, uint32_t idx, float interp) {
  if(idx >= scalars.size())return scalars.back();
  int idxNext = idx + 1;
  if(idxNext >= scalars.size())return scalars.back();
  if(interp < 0.0001f)return scalars[idx];
  if(interp > 0.9999f)return scalars[idxNext];
  return scalars[idx] + (scalars[idxNext] - scalars[idx]) * interp;
}
