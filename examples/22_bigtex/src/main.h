/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

template<typename T>
constexpr T* unached(T *ptr) {
  return (T*)UncachedAddr(ptr);
}

constexpr uint32_t SCREEN_WIDTH = 320;
constexpr uint32_t SCREEN_HEIGHT = 240;

constexpr uint32_t TEX_BASE_ADDR = 0x8040'0000;
static_assert((TEX_BASE_ADDR & 0x000F'FFFF) == 0, "Low 20bits must be zero");

consteval float operator ""    _s(long double x) { return static_cast<float>(x); }
consteval float operator ""   _ms(long double x) { return static_cast<float>(x / 1000.0); }

consteval float operator ""    _s(unsigned long long x) { return static_cast<float>(x); }
consteval float operator ""   _ms(unsigned long long x) { return static_cast<float>(x) / 1000.0f; }

struct State{
  constexpr static uint32_t DRAW_MODE_DEF = 0;
  constexpr static uint32_t DRAW_MODE_UV  = 1;
  constexpr static uint32_t DRAW_MODE_MAT = 2;

  uint32_t frame{0};
  uint32_t frameIdx{0};
  float fps{0};
  int currSkybox{0};
  bool menuOpen{true};
  bool drawShade{true};
  bool drawMap{true};
  bool slowCam{false};
  bool limitFPS{false};
  int mapModel{0};
  int drawMode{0};
};
extern State state;