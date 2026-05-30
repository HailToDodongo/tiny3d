/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once

#include <t3d/t3d.h>


struct Camera
{
  T3DViewport viewport{};
  fm_vec3_t pos{};
  fm_vec3_t target{};
  float fov{};
  float near{};
  float far{};

  uint8_t needsProjUpdate{false};

  Camera();

  void update(float deltaTime);
  void attach();

  void move(fm_vec3_t dir) {
    target += dir;
    pos += dir;
  }

  [[nodiscard]] const fm_vec3_t &getTarget() const { return target; }
  [[nodiscard]] const fm_vec3_t &getPos() const { return pos; }

  [[nodiscard]] fm_vec3_t getDirection() const {
    auto dir = target - pos;
    fm_vec3_norm(&dir, &dir);
    return dir;
  }

  const T3DFrustum &getFrustum() const {
    return viewport.viewFrustum;
  }
};