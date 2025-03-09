/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace Memory {
  struct FrameBuffers {
    surface_t color[3]{};
    surface_t uv[3]{};
    surface_t shade[3]{};
    surface_t *depth[2]{};
  };

  void dumpHeap(const char* name = nullptr);
  surface_t* getZBuffer(uint32_t idx = 0);

  FrameBuffers allocOptimalFrameBuffers();
}