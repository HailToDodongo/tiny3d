/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include "fbBlend.h"

namespace FbCPU {
  void applyTextures(uint64_t *fbTexIn, uint16_t *buffOut, uint32_t buffSize, const FbBlend &fbBlend);
  void applyTexturesUV(uint64_t *fbTexIn, uint16_t *buffOut, uint32_t buffSize);
  void applyTexturesMat(uint64_t *fbTexIn, uint16_t *buffOut, uint32_t buffSize);
}