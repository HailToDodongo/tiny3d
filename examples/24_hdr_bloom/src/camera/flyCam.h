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

  T3DVec3 camPos{};
  float camRotXCurr{};
  float camRotYCurr{};

  void update(float deltaTime);
};