/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace Debug
{
  void init();

  void printStart();
  float print(float x, float y, const char* str);
  float printf(float x, float y, const char *fmt, ...);

  void destroy();
}