/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "debugDraw.h"

namespace
{
  sprite_t *font{};
  rspq_block_t *dplSetup{};
}

void Debug::init() {
  font = sprite_load("rom:/font.ia4.sprite");

  rspq_block_begin();
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_sync_load();

    rdpq_mode_begin();
      rdpq_set_mode_standard();
      rdpq_mode_blender(0);
      rdpq_mode_persp(false);
      rdpq_mode_antialias(AA_NONE);
      rdpq_mode_alphacompare(1);
    rdpq_mode_end();

    rdpq_sprite_upload(TILE0, font, NULL);
  dplSetup = rspq_block_end();
}

void Debug::destroy() {
  sprite_free(font);
  rspq_block_free(dplSetup);
}

void Debug::printStart() {
  rspq_block_run(dplSetup);
}

float Debug::print(float x, float y, const char *str) {
  constexpr int width = 8;
  constexpr int height = 8;
  int s = 0;

  while(*str) {
    uint8_t c = *str;
    if(c != ' ')
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
