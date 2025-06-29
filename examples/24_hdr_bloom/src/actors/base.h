/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace Actor
{
  class Base
  {
    private:

    protected:
      fm_vec3_t pos{};

    public:
      constexpr static uint32_t FLAG_DISABLED = (1 << 0);

      uint32_t flags{0};

      virtual ~Base() {}
      virtual void update(float deltaTime) = 0;

      virtual void draw3D(float deltaTime) {}
      virtual void drawPTX(float deltaTime) {}
  };
}