/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "fbCpuDraw.h"
#include "../main.h"
#include "../rsp/rspFX.h"

#pragma GCC push_options
#pragma GCC optimize ("-O3")

extern "C" {
  // see: ./tex.S
  extern uint32_t applyTexture(uint32_t fbTexIn, uint32_t fbTexInEnd, uint32_t fbOut64);
}

void FbCPU::applyTextures(uint64_t *fbTexIn, uint16_t *buffOut, uint32_t buffSize, const FbBlend &fbBlend)
{
  uint32_t quarterSlice = buffSize / SHADE_BLEND_SLICES / 3;
  uint32_t stepSizeTexIn = quarterSlice * 2;
  uint32_t stepSizeTexInRSP = quarterSlice * 1;

  uint32_t ptrInPos = (uint32_t)(fbTexIn);
  uint32_t ptrOutPos = (uint32_t) CachedAddr(buffOut);

  // Texture look-up
  // This starts the CPU/RSP tasks to convert UVs into actual pixels.
  // RSP and CPU are doing this in slices, so that they can work in parallel...
  if(state.drawShade && state.drawMap)
  {
    for(int p=0; p<SHADE_BLEND_SLICES; ++p) {
      if(p % 4 == 0)
      {
        RSP::FX::fillTextures(ptrInPos, ptrInPos + stepSizeTexInRSP, ptrOutPos);
        rspq_flush(); // <- make sure to flush to guarantee that the RSP is busy
        ptrInPos += stepSizeTexInRSP;
        ptrOutPos += stepSizeTexInRSP / 2;
        applyTexture(ptrInPos, ptrInPos + stepSizeTexIn, ptrOutPos);
        ptrInPos += stepSizeTexIn;
        ptrOutPos += stepSizeTexIn / 2;
      } else {
        applyTexture(ptrInPos, ptrInPos + quarterSlice*3, ptrOutPos);
        ptrInPos += quarterSlice * 3;
        ptrOutPos += quarterSlice * 3 / 2;
      }
      data_cache_hit_writeback_invalidate((char*)CachedAddr(ptrOutPos) - 0x1000, 0x1000);
      // ...in the case of shading enabled, we also start the final blend inbetween,
      // this can once again run in parallel
      fbBlend.blend(p);

      rspq_flush();
    }
  } else {
    // version without shading, this is the same as above without the RDP tasks inbetween
    stepSizeTexIn =  buffSize / SHADE_BLEND_SLICES;
    for(int p=0; p<SHADE_BLEND_SLICES; ++p)
    {
      if(p % 4 == 0) {
        RSP::FX::fillTextures(
          ptrInPos, ptrInPos + stepSizeTexIn, ptrOutPos
        );
        rspq_flush();
      } else {
        applyTexture(ptrInPos, ptrInPos + stepSizeTexIn, ptrOutPos);
        data_cache_hit_writeback_invalidate((char*)CachedAddr(ptrOutPos + stepSizeTexIn/2) - 0x1000, 0x1000);
      }

      ptrOutPos += stepSizeTexIn / 2;
      ptrInPos += stepSizeTexIn;
    }
  }
}

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