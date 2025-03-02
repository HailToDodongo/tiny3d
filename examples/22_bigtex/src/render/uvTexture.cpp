/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "uvTexture.h"

namespace
{
  constexpr uint32_t SWIZZLE_SIZE = 4;

  /**
   * Generates two slices (vertical and horizontal) that when combined form a gradient texture.
   * This is effectively a "UV identity" texture, meaning by sampling with a given UV it will return the UV itself as color.
   * On top, the coordinates are swizzled in a 4x4 block pattern, this mirrors the way textures are encoded.
   * Swizzling is done to improve cache locality by making it more likely nearby pixels one both X/Y axis are close together.
   * @param texGradU
   * @param texGradV
   */
  void generateUvTexture(surface_t &texGradU, surface_t &texGradV)
  {
    auto data = (uint32_t*)texGradU.buffer;
    for(uint32_t y = 0; y < SWIZZLE_SIZE; ++y) {
      uint32_t val = (y % SWIZZLE_SIZE) * SWIZZLE_SIZE;
      for(uint32_t i = 0; i < (256/SWIZZLE_SIZE); i+=SWIZZLE_SIZE) {
        for(uint32_t sub=0; sub<SWIZZLE_SIZE; ++sub) {
          *(data++) = ((val+sub) << 8) & 0xFF00;
        }
        val += SWIZZLE_SIZE * SWIZZLE_SIZE;
      }
    }

    data = (uint32_t*)texGradV.buffer;
    uint32_t val = 0;

    for(uint32_t i = 0; i < (256/SWIZZLE_SIZE); i++) {
      for(uint32_t sub=0; sub<SWIZZLE_SIZE; ++sub) {
        *(data++) = ((val+sub) << 16) & 0xFF0000;
      }
      val += SWIZZLE_SIZE;
    }
  }
}

UVTexture::UVTexture() {
  texU = surface_alloc(FMT_RGBA32, 256 / SWIZZLE_SIZE, SWIZZLE_SIZE);
  texV = surface_alloc(FMT_RGBA32, SWIZZLE_SIZE, 256 / SWIZZLE_SIZE);
  generateUvTexture(texU, texV);

  rspq_block_begin();
    rdpq_texparms_t texParamsU{};
    texParamsU.s.repeats = REPEAT_INFINITE;
    texParamsU.t.repeats = REPEAT_INFINITE;
    auto texParamsV = texParamsU;
    texParamsV.s.scale_log = 6;
    texParamsV.t.scale_log = 2;

    rdpq_tex_multi_begin();
      rdpq_tex_upload(TILE0, &texU, &texParamsU);
      rdpq_tex_upload(TILE1, &texV, &texParamsV);
    rdpq_tex_multi_end();
  dpl = rspq_block_end();
}

UVTexture::~UVTexture() {
  surface_free(&texU);
  surface_free(&texV);
  rspq_block_free(dpl);
}
