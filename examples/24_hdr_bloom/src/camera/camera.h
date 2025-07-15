/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once

#include <t3d/t3d.h>
#include <t3d/t3dmath.h>

struct Camera
{
  T3DViewport viewports[3]{};
  T3DVec3 pos{};
  T3DVec3 target{};
  float fov{};
  float near{};
  float far{};

  uint8_t vpIdx{0};
  uint8_t needsProjUpdate{false};

  Camera();

  void update(float deltaTime);
  void attach();

  void move(T3DVec3 dir) {
    target += dir;
    pos += dir;
  }

  [[nodiscard]] const fm_vec3_t &getTarget() const { return target; }
  [[nodiscard]] const fm_vec3_t &getPos() const { return pos; }

  [[nodiscard]] fm_vec3_t getDirection() const {
    auto dir = target - pos;
    t3d_vec3_norm(&dir);
    return dir;
  }

  const T3DFrustum &getFrustum() {
    return viewports[vpIdx].viewFrustum;
  }
};