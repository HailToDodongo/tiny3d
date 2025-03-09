/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "debugDraw.h"
#include "../main.h"
#include <t3d/t3d.h>
#include <vector>

namespace
{
  sprite_t *font{};
  rspq_block_t *dplSetup{};

  struct Line {
    T3DVec3 a{};
    T3DVec3 b{};
    uint16_t color;
    uint16_t _padding;
  };

  std::vector<Line> lines{};

  void debugDrawLine(uint16_t *fb, int px0, int py0, int px1, int py1, uint16_t color)
  {
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    if((px0 > width + 200) || (px1 > width + 200) ||
       (py0 > height + 200) || (py1 > height + 200)) {
      return;
    }

    float pos[2]{(float)px0, (float)py0};
    int dx = px1 - px0;
    int dy = py1 - py0;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    if(steps <= 0)return;
    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    for(int i=0; i<steps; ++i)
    {
      if((i%3 != 0) && pos[1] >= 0 && pos[1] < height && pos[0] >= 0 && pos[0] < width) {
        fb[(int)pos[1] * width + (int)pos[0]] = color;
      }
      pos[0] += xInc;
      pos[1] += yInc;
    }
  }
}

void Debug::init() {
  lines = {};
  font = sprite_load("rom:/font.ia4.sprite");

  rspq_block_begin();
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_sync_load();

    rdpq_mode_begin();
      rdpq_mode_blender(0);
      rdpq_mode_persp(false);
      rdpq_mode_antialias(AA_NONE);
      rdpq_mode_alphacompare(1);
      rdpq_mode_combiner(RDPQ_COMBINER1((0,0,0,TEX0), (0,0,0,TEX0)));
    rdpq_mode_end();

    rdpq_sprite_upload(TILE0, font, NULL);
  dplSetup = rspq_block_end();
}

void Debug::destroy() {
  lines.clear();
  lines.shrink_to_fit();

  sprite_free(font);
  rspq_block_free(dplSetup);
}


void Debug::drawLine(const T3DVec3 &a, const T3DVec3 &b, color_t color) {
  if(lines.size() > 1000)return;
  lines.push_back({a, b, color_to_packed16(color)});
}

void Debug::draw(uint16_t *fb) {
  if(lines.empty())return;
  debugf("Drawing %d lines\n", lines.size());
  //rspq_wait();

  for(auto &line : lines) {
    t3d_viewport_calc_viewspace_pos(nullptr, &line.a, &line.a);
    t3d_viewport_calc_viewspace_pos(nullptr, &line.b, &line.b);
  }

  float maxX = SCREEN_WIDTH;
  float maxY = SCREEN_HEIGHT;
  for(auto &line : lines) {
    if(line.a.x < 0 && line.b.x < 0)continue;
    if(line.a.y < 0 && line.b.y < 0)continue;
    if(line.a.x > maxX && line.b.x > maxX)continue;
    if(line.a.y > maxY && line.b.y > maxY)continue;
    debugDrawLine(fb, line.a.x, line.a.y, line.b.x, line.b.y, line.color);
  }

  lines.clear();
}

void Debug::drawAABB(const T3DVec3 &p, const T3DVec3 &halfExtend, color_t color) {
  T3DVec3 a = p - halfExtend;
  T3DVec3 b = p + halfExtend;
  // draw all 12 edges
  drawLine(a, T3DVec3{b.x, a.y, a.z}, color);
  drawLine(a, T3DVec3{a.x, b.y, a.z}, color);
  drawLine(a, T3DVec3{a.x, a.y, b.z}, color);
  drawLine(T3DVec3{b.x, a.y, a.z}, T3DVec3{b.x, b.y, a.z}, color);
  drawLine(T3DVec3{b.x, a.y, a.z}, T3DVec3{b.x, a.y, b.z}, color);
  drawLine(T3DVec3{a.x, b.y, a.z}, T3DVec3{b.x, b.y, a.z}, color);
  drawLine(T3DVec3{a.x, b.y, a.z}, T3DVec3{a.x, b.y, b.z}, color);
  drawLine(T3DVec3{a.x, a.y, b.z}, T3DVec3{b.x, a.y, b.z}, color);
  drawLine(T3DVec3{a.x, a.y, b.z}, T3DVec3{a.x, b.y, b.z}, color);
  drawLine(T3DVec3{b.x, b.y, a.z}, T3DVec3{b.x, b.y, b.z}, color);
  drawLine(T3DVec3{b.x, b.y, a.z}, T3DVec3{a.x, b.y, a.z}, color);
  drawLine(T3DVec3{b.x, b.y, b.z}, T3DVec3{a.x, b.y, b.z}, color);
}

void Debug::printStart() {
  rspq_block_run(dplSetup);
}

float Debug::print(float x, float y, const char *str) {
  int width = 8;
  int height = 8;
  int s = 0;

  while(*str) {
    uint8_t c = *str;
    if(c != ' ' && c != '\n')
    {
      if(c >= 'a' && c <= 'z')c &= ~0x20;
      s = (c - 33) * width;
      rdpq_texture_rectangle_raw(TILE0, x, y, x+width, y+height, s, 0, 1, 1);
    }
    ++str;
    x += 7;
  }
  return x;
}

float Debug::printf(float x, float y, const char *fmt, ...) {
  if(x > 320-8)return x;
  char buffer[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, 128, fmt, args);
  va_end(args);
  return Debug::print(x, y, buffer);
}
