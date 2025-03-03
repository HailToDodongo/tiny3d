/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

class UVTexture
{
  private:
    surface_t texU{};
    surface_t texV{};
    rspq_block_t *dpl{};

  public:
    UVTexture();
    ~UVTexture();

    inline void use() {
      rspq_block_run(dpl);
    }
};