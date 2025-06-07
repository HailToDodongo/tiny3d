/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include <t3d/t3dmath.h>

namespace RspFX
{
  void init();
  void hdrBlit(void* rgba32In, void *rgba16Out, float factor);
  void blur(void* rgba32In, void* rgba32Out, float brightness);
}
