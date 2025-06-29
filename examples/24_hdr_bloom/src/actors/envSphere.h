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
  class EnvSphere : public Base
  {
    public:
      struct Args
      {
        uint32_t type;
      };

    private:
      RingMat4FP matFP{};
      float timer{};
      Args args{};

    public:
      EnvSphere(const fm_vec3_t &_pos, const Args &_args);
      ~EnvSphere();

      void update(float deltaTime) final;
      void draw3D(float deltaTime) final;
  };
}