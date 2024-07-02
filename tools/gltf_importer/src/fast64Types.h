/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once
#include "structs.h"

namespace {
  // Maps the rendermodes to the alpha modes
  constexpr uint32_t F64_RENDER_MODE_1_TO_BLENDER[] = {
    RDP::BLEND::NONE, // Background
    RDP::BLEND::NONE, // Opaque
    RDP::BLEND::NONE, // Opaque Decal
    RDP::BLEND::NONE, // Opaque Intersecting
    RDP::BLEND::NONE, // Cutout
    RDP::BLEND::MULTIPLY, // Transparent
    RDP::BLEND::MULTIPLY, // Transparent Decal
    RDP::BLEND::MULTIPLY, // Transparent Intersecting
    RDP::BLEND::NONE, // Fog Shade
    RDP::BLEND::NONE, // Fog Primitive
    RDP::BLEND::NONE, // Pass
    RDP::BLEND::NONE, // Add
    RDP::BLEND::NONE, // No Op
    RDP::BLEND::NONE, // Opaque (No AA)
    RDP::BLEND::NONE, // Opaque Decal (No AA)
    RDP::BLEND::MULTIPLY, // Transparent (No AA)
    RDP::BLEND::MULTIPLY, // Transparent Decal (No AA)
    RDP::BLEND::NONE, // Opaque (No AA, No ZBuf)
    RDP::BLEND::NONE, // Cloud (No AA)
    RDP::BLEND::NONE, // Terrain
  };

  constexpr uint32_t F64_RENDER_MODE_2_TO_BLENDER[] = {
    RDP::BLEND::NONE, // Background
    RDP::BLEND::NONE, // Opaque
    RDP::BLEND::NONE, // Opaque Decal
    RDP::BLEND::NONE, // Opaque Intersecting
    RDP::BLEND::NONE, // Cutout
    RDP::BLEND::MULTIPLY, // Transparent
    RDP::BLEND::MULTIPLY, // Transparent Decal
    RDP::BLEND::MULTIPLY, // Transparent Intersecting
    RDP::BLEND::NONE, // Add
    RDP::BLEND::NONE, // No Op
    RDP::BLEND::NONE, // Opaque (No AA)
    RDP::BLEND::NONE, // Opaque Decal (No AA)
    RDP::BLEND::MULTIPLY, // Transparent (No AA)
    RDP::BLEND::MULTIPLY, // Transparent Decal (No AA)
    RDP::BLEND::NONE, // Opaque (No AA, No ZBuf)
    RDP::BLEND::NONE, // Cloud (No AA)
    RDP::BLEND::NONE, // Terrain
  };

  constexpr uint64_t F64_RENDER_MODE_1_TO_OTHERMODE[] = {
  /* Background */                0,
  /* Opaque */                    0,
  /* Opaque Decal */              RDP::SOM::ZMODE_DECAL,
  /* Opaque Intersecting */       RDP::SOM::ZMODE_INTERPEN,
  /* Cutout */                    RDP::SOM::ALPHA_COMPARE,
  /* Transparent */               0,
  /* Transparent Decal */         RDP::SOM::ZMODE_DECAL,
  /* Transparent Intersecting */  RDP::SOM::ZMODE_INTERPEN,
  /* Fog Shade */                 0,
  /* Fog Primitive */             0,
  /* Pass */                      0,
  /* Add */                       0,
  /* No Op */                     0,
  /* Opaque (No AA) */            0,
  /* Opaque Decal (No AA) */      RDP::SOM::ZMODE_DECAL,
  /* Transparent (No AA) */       0,
  /* Transparent Decal (No AA) */ RDP::SOM::ZMODE_DECAL,
  /* Opaque (No AA, No ZBuf) */   0,
  /* Cloud (No AA) */             0,
  /* Terrain */                   0,
  };

  constexpr uint64_t F64_RENDER_MODE_2_TO_OTHERMODE[] = {
  /* Background */                0,
  /* Opaque */                    0,
  /* Opaque Decal */              RDP::SOM::ZMODE_DECAL,
  /* Opaque Intersecting */       RDP::SOM::ZMODE_INTERPEN,
  /* Cutout */                    RDP::SOM::ALPHA_COMPARE,
  /* Transparent */               0,
  /* Transparent Decal */         RDP::SOM::ZMODE_DECAL,
  /* Transparent Intersecting */  RDP::SOM::ZMODE_INTERPEN,
  /* Add */                       0,
  /* No Op */                     0,
  /* Opaque (No AA) */            0,
  /* Opaque Decal (No AA) */      RDP::SOM::ZMODE_DECAL,
  /* Transparent (No AA) */       0,
  /* Transparent Decal (No AA) */ RDP::SOM::ZMODE_DECAL,
  /* Opaque (No AA, No ZBuf) */   0,
  /* Cloud (No AA) */             0,
  /* Terrain */                   0,
  };
}