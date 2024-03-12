/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once
#include "structs.h"

namespace {
  // Maps the rendermodes to the alpha modes
  constexpr uint8_t F64_RENDER_MODE_1_TO_ALPHA[] = {
    AlphaMode::DEFAULT, // Background
    AlphaMode::OPAQUE, // Opaque
    AlphaMode::OPAQUE, // Opaque Decal
    AlphaMode::OPAQUE, // Opaque Intersecting
    AlphaMode::CUTOUT, // Cutout
    AlphaMode::TRANSP, // Transparent
    AlphaMode::TRANSP, // Transparent Decal
    AlphaMode::TRANSP, // Transparent Intersecting
    AlphaMode::OPAQUE, // Fog Shade
    AlphaMode::OPAQUE, // Fog Primitive
    AlphaMode::INVALID, // Pass
    AlphaMode::INVALID, // Add
    AlphaMode::INVALID, // No Op
    AlphaMode::OPAQUE, // Opaque (No AA)
    AlphaMode::OPAQUE, // Opaque Decal (No AA)
    AlphaMode::TRANSP, // Transparent (No AA)
    AlphaMode::TRANSP, // Transparent Decal (No AA)
    AlphaMode::OPAQUE, // Opaque (No AA, No ZBuf)
    AlphaMode::INVALID, // Cloud (No AA)
    AlphaMode::INVALID, // Terrain
  };

  constexpr uint8_t F64_RENDER_MODE_2_TO_ALPHA[] = {
    AlphaMode::DEFAULT, // Background
    AlphaMode::OPAQUE, // Opaque
    AlphaMode::OPAQUE, // Opaque Decal
    AlphaMode::OPAQUE, // Opaque Intersecting
    AlphaMode::CUTOUT, // Cutout
    AlphaMode::TRANSP, // Transparent
    AlphaMode::TRANSP, // Transparent Decal
    AlphaMode::TRANSP, // Transparent Intersecting
    AlphaMode::INVALID, // Add
    AlphaMode::INVALID, // No Op
    AlphaMode::OPAQUE, // Opaque (No AA)
    AlphaMode::OPAQUE, // Opaque Decal (No AA)
    AlphaMode::TRANSP, // Transparent (No AA)
    AlphaMode::TRANSP, // Transparent Decal (No AA)
    AlphaMode::OPAQUE, // Opaque (No AA, No ZBuf)
    AlphaMode::INVALID, // Cloud (No AA)
    AlphaMode::INVALID, // Terrain
  };
}