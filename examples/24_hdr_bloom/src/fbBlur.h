/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

class FbBlur
{
  private:
    rspq_block_t *blendBlock{};
    surface_t surfBlurA{};
    surface_t surfBlurB{};

  public:
    FbBlur();
    ~FbBlur();
    surface_t &blur(surface_t& src);
};
