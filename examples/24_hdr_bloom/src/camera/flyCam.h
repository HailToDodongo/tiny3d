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

  void update(float deltaTime);
};