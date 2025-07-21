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
  class PointGlobe : public Base
  {
    public:
      struct Args
      {
        float scale{};
      };

    private:
      PTSystem particles{};
      float timer{};
      float timerNoise{};
      float timerFlicker{};
      float actualScale{};
      int lastDrawnPartCount{};
      Args args{};

    public:
      PointGlobe(const fm_vec3_t &_pos, const Args &_args);
      ~PointGlobe();

      void update(float deltaTime) final;
      void draw3D(float deltaTime) final {}
      void drawPTX(float deltaTime) final;
  };
}