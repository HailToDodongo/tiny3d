/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>


namespace RspFX
{
  void init();
  void hdrBlit(void* rgba32In, void *rgba16Out, void* rgba32BloomIn, float factor);
  void downscale(void* rgba32In, void* rgba32Out);
  void blur(void* rgba32In, void* rgba32Out, float brightness, float threshold);
}
