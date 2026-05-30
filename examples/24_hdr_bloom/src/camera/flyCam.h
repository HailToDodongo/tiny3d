/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include "camera.h"

struct FlyCam
{
  Camera &cam;
  float camRotX{};
  float camRotY{};

  fm_vec3_t camPos{};
  float camRotXCurr{};
  float camRotYCurr{};

  void update(float deltaTime);
};