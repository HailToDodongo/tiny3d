/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "base.h"
#include <libdragon.h>
#include <t3d/t3d.h>
#include "../memory/matrixManager.h"
#include "../render/ptSystem.h"

namespace Actor
{
  class MagicSpell : public Base
  {
    public:
      struct Args
      {
        float scale{};
        color_t color{};
      };

    private:
      PTSystem particles{256};
      int8_t orgPosX[256]{};
      int8_t orgPosZ[256]{};
      int8_t displace[255]{};

      RingMat4FP matFP{};
      Args args{};
      float timer{};

    public:
      MagicSpell(const fm_vec3_t &_pos, const Args &_args);
      ~MagicSpell();

      void update(float deltaTime) final;
      void draw3D(float deltaTime) final;
      void drawPTX(float deltaTime) final;
  };
}