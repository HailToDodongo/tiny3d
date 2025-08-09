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
  class MagicSphere : public Base
  {
    public:
      struct Args
      {
        float scale{};
        color_t color{};
      };

    private:
      RingMat4FP matFP{};
      float timer{};
      Args args{};

    public:
      MagicSphere(const fm_vec3_t &_pos, const Args &_args);
      ~MagicSphere();

      void update(float deltaTime) final;
      void draw3D(float deltaTime) final;
  };
}