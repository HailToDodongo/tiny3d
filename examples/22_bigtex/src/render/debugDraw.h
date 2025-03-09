/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include <t3d/t3dmath.h>

namespace Debug
{
  void init();

  void drawAABB(const T3DVec3 &p, const T3DVec3 &halfExtend, color_t color = {0xFF, 0xFF, 0xFF, 0xFF});
  void drawLine(const T3DVec3 &a, const T3DVec3 &b, color_t color = {0xFF,0xFF,0xFF,0xFF});
  void draw(uint16_t *fb);

  void printStart();
  float print(float x, float y, const char* str);
  float printf(float x, float y, const char *fmt, ...);

  void destroy();
}