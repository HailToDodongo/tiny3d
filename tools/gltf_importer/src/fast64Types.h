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

  constexpr uint8_t F64_RENDER_MODE_1_TO_ZMODE[] = {
    ZMode::OPAQUE, // Background
    ZMode::OPAQUE, // Opaque
    ZMode::DECAL, // Opaque Decal
    ZMode::INTERSECT, // Opaque Intersecting
    ZMode::OPAQUE, // Cutout
    ZMode::OPAQUE, // Transparent
    ZMode::DECAL, // Transparent Decal
    ZMode::INTERSECT, // Transparent Intersecting
    ZMode::OPAQUE, // Fog Shade
    ZMode::OPAQUE, // Fog Primitive
    ZMode::OPAQUE, // Pass
    ZMode::OPAQUE, // Add
    ZMode::OPAQUE, // No Op
    ZMode::OPAQUE, // Opaque (No AA)
    ZMode::DECAL, // Opaque Decal (No AA)
    ZMode::OPAQUE, // Transparent (No AA)
    ZMode::DECAL, // Transparent Decal (No AA)
    ZMode::OPAQUE, // Opaque (No AA, No ZBuf)
    ZMode::OPAQUE, // Cloud (No AA)
    ZMode::OPAQUE, // Terrain
  };

  constexpr uint8_t F64_RENDER_MODE_2_TO_ZMODE[] = {
    ZMode::OPAQUE, // Background
    ZMode::OPAQUE, // Opaque
    ZMode::DECAL, // Opaque Decal
    ZMode::INTERSECT, // Opaque Intersecting
    ZMode::OPAQUE, // Cutout
    ZMode::OPAQUE, // Transparent
    ZMode::DECAL, // Transparent Decal
    ZMode::INTERSECT, // Transparent Intersecting
    ZMode::OPAQUE, // Add
    ZMode::OPAQUE, // No Op
    ZMode::OPAQUE, // Opaque (No AA)
    ZMode::DECAL, // Opaque Decal (No AA)
    ZMode::OPAQUE, // Transparent (No AA)
    ZMode::DECAL, // Transparent Decal (No AA)
    ZMode::OPAQUE, // Opaque (No AA, No ZBuf)
    ZMode::OPAQUE, // Cloud (No AA)
    ZMode::OPAQUE, // Terrain
  };

  constexpr uint64_t F64_RENDER_MODE_1_TO_OTHERMODE[] = {
  /* Background */                0,
  /* Opaque */                    0,
  /* Opaque Decal */              0,
  /* Opaque Intersecting */       0,
  /* Cutout */                    RDP::SOM::ALPHA_COMPARE,
  /* Transparent */               0,
  /* Transparent Decal */         0,
  /* Transparent Intersecting */  0,
  /* Fog Shade */                 0,
  /* Fog Primitive */             0,
  /* Pass */                      0,
  /* Add */                       0,
  /* No Op */                     0,
  /* Opaque (No AA) */            0,
  /* Opaque Decal (No AA) */      0,
  /* Transparent (No AA) */       0,
  /* Transparent Decal (No AA) */ 0,
  /* Opaque (No AA, No ZBuf) */   0,
  /* Cloud (No AA) */             0,
  /* Terrain */                   0,
  };

  constexpr uint64_t F64_RENDER_MODE_2_TO_OTHERMODE[] = {
  /* Background */                0,
  /* Opaque */                    0,
  /* Opaque Decal */              0,
  /* Opaque Intersecting */       0,
  /* Cutout */                    RDP::SOM::ALPHA_COMPARE,
  /* Transparent */               0,
  /* Transparent Decal */         0,
  /* Transparent Intersecting */  0,
  /* Add */                       0,
  /* No Op */                     0,
  /* Opaque (No AA) */            0,
  /* Opaque Decal (No AA) */      0,
  /* Transparent (No AA) */       0,
  /* Transparent Decal (No AA) */ 0,
  /* Opaque (No AA, No ZBuf) */   0,
  /* Cloud (No AA) */             0,
  /* Terrain */                   0,
  };
}