/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "base.h"
#include "../main.h"
#include "../scene/scene.h"

bool Actor::Base::checkFrustumAABB(const fm_vec3_t &aabbMin, const fm_vec3_t &aabbMax) const
{
  auto &fr = state.activeScene->getCam().getFrustum();
  return t3d_frustum_vs_aabb(&fr, &aabbMin, &aabbMax);
}

bool Actor::Base::checkFrustumSphere(const fm_vec3_t &center, float radius) const
{
  auto &fr = state.activeScene->getCam().getFrustum();
  return t3d_frustum_vs_sphere(&fr, &center, radius);
}