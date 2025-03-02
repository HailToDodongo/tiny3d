/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "fbCpuDraw.h"

#pragma GCC push_options
#pragma GCC optimize ("-O3")

void FbCPU::applyTexturesUV(uint64_t *fbTexIn, uint16_t *buffOut, uint32_t buffSize)
{
  auto fbTexInEnd = fbTexIn + (buffSize / 8);
  auto *fbOut64 = (uint16_t*) CachedAddr(buffOut);

  while(fbTexIn < fbTexInEnd)
  {
    asm("cache %0,(%1)\n"::"i" (((0x3 << 2) | 0x1)), "r" (fbOut64));

    for(uint32_t i=0; i<4; ++i) {
      uint8_t u0 = ((uint8_t*)fbTexIn)[1];
      uint8_t v0 = ((uint8_t*)fbTexIn)[2];
      uint8_t u1 = ((uint8_t*)fbTexIn)[5];
      uint8_t v1 = ((uint8_t*)fbTexIn)[6];

      fbOut64[0] = color_to_packed16({0, u0, v0, 0});
      fbOut64[1] = color_to_packed16({0, u1, v1, 0});
      fbOut64+=2;
      ++fbTexIn;
    }
  }
}

void FbCPU::applyTexturesMat(uint64_t *fbTexIn, uint16_t *buffOut, uint32_t buffSize)
{
  auto fbTexInEnd = fbTexIn + (buffSize / 8);
  auto *fbOut64 = (uint16_t*) CachedAddr(buffOut);

  uint16_t MAT_COLORS[14] = {
      color_to_packed16(color_from_packed32(0xFFFFFF'FF)),
      color_to_packed16(color_from_packed32(0xFFFF00'FF)),
      color_to_packed16(color_from_packed32(0x808000'FF)),
      color_to_packed16(color_from_packed32(0x00FF00'FF)),
      color_to_packed16(color_from_packed32(0xFF0000'FF)),
      color_to_packed16(color_from_packed32(0x00FFFF'FF)),
      color_to_packed16(color_from_packed32(0xAA00FF'FF)),
      color_to_packed16(color_from_packed32(0x003060'FF)),
      color_to_packed16(color_from_packed32(0x008080'FF)),
      color_to_packed16(color_from_packed32(0x9090C0'FF)),
      color_to_packed16(color_from_packed32(0x0000FF'FF)),
      color_to_packed16(color_from_packed32(0x800080'FF)),
      color_to_packed16(color_from_packed32(0x405080'FF)),
      color_to_packed16(color_from_packed32(0x20F060'FF)),
  };

  while(fbTexIn < fbTexInEnd)
  {
    asm("cache %0,(%1)\n"::"i" (((0x3 << 2) | 0x1)), "r" (fbOut64));

    for(uint32_t i=0; i<4; ++i) {
      uint8_t mat0 = ((uint8_t*)fbTexIn)[0] - 0x40;
      uint8_t mat1 = ((uint8_t*)fbTexIn)[4] - 0x40;

      fbOut64[0] = MAT_COLORS[mat0];
      fbOut64[1] = MAT_COLORS[mat1];
      fbOut64+=2;
      ++fbTexIn;
    }
  }
}

  #pragma GCC pop_options