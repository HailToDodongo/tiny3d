/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace FbCPU {
  void applyTexturesUV(uint64_t *fbTexIn, uint16_t *buffOut, uint32_t buffSize);
  void applyTexturesMat(uint64_t *fbTexIn, uint16_t *buffOut, uint32_t buffSize);
}