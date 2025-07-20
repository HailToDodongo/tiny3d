/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "base.h"
#include <libdragon.h>
#include <t3d/t3d.h>
#include "../memory/matrixManager.h"

namespace Actor
{
  class LightBlock : public Base
  {
    public:
      struct Args
      {
        color_t color{};
        uint8_t index{};
      };

    private:
      RingMat4FP matFP{};
      Args args{};
      float timer{};

    public:
      LightBlock(const fm_vec3_t &_pos, const Args &_args);
      ~LightBlock();

      void update(float deltaTime) final;
      void draw3D(float deltaTime) final;
  };
}