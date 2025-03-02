/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

constexpr int SHADE_BLEND_SLICES = 16;

class FbBlend
{
  private:
    rspq_block_t *blendBlocks[SHADE_BLEND_SLICES]{nullptr};
    surface_t placeShade{};
    surface_t placeTex{};

  public:
    FbBlend();
    ~FbBlend();

    inline void blend(int idx) {
      rspq_block_run(blendBlocks[idx]);
    }
};