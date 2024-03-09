/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "t3ddebug.h"
#include <libdragon.h>
#include <stdarg.h>

static sprite_t *spriteFont = NULL;

void t3d_debug_print_init() {
  if(!spriteFont) {
    spriteFont = sprite_load("rom:/font.ia4.sprite");
  }
}

void t3d_debug_print_start() {
  rdpq_sync_pipe();
  rdpq_sync_tile();
  rdpq_sync_load();

  rdpq_set_mode_standard();
  rdpq_mode_antialias(AA_NONE);
  rdpq_mode_combiner(RDPQ_COMBINER1((TEX0,0,PRIM,0), (TEX0,0,PRIM,0)));
  rdpq_mode_alphacompare(1);
  rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));

  rdpq_sprite_upload(TILE0, spriteFont, NULL);
}

void t3d_debug_print(float x, float y, const char* str) {
  int width = 8;
  int height = 12;
  int s = 0;

  while(*str) {
    char c = *str;
    if(c != ' ' && c != '\n')
    {
           if(c >= 'A' && c <= '_')s = (c - 'A' + 27) * width;
      else if(c >= 'a' && c <= 'z')s = (c - 'a' + 27+31) * width;
      else if(c >= '!' && c <= '@')s = (c - '!') * width;
      rdpq_texture_rectangle_raw(TILE0, x, y, x+width, y+height, s, 0, 1, 1);
    }
    ++str;
    x += 8;
  }
}

void t3d_debug_printf(float x, float y, const char *fmt, ...) {
  char buffer[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, 256, fmt, args);
  va_end(args);
  t3d_debug_print(x, y, buffer);
}
