/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include <functional>

namespace SwapChain
{
  using RenderPassCB = void(*)(uint32_t fbIndex);
  using RenderPassDrawTask = std::function<void(surface_t* fb, uint32_t fbIndex, RenderPassCB done)>;

  struct SwapChainConf {
    surface_t *frameBuffers;
    uint32_t frameBufferCount;
  };

  void init(const SwapChainConf &conf);

  void nextFrame();
  void drain();
  void setFrameSkip(uint32_t skip);

  void setDrawPass(RenderPassDrawTask task);
  void start();

  surface_t *getFrameBuffer(uint32_t idx);
}