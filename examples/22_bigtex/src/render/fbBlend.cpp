/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "fbBlend.h"
#include "../main.h"

FbBlend::FbBlend()
{
  placeShade = surface_make_placeholder(1, FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH * 2);
  placeTex = surface_make_placeholder(2, FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH * 2);

  rspq_block_begin();
    rdpq_mode_begin();
      rdpq_mode_blender(0);
      rdpq_mode_combiner(RDPQ_COMBINER2(
        (1, TEX0, TEX1, 0), (0,0,0,TEX0),
        (0, 0, 0, COMBINED), (0,0,0,COMBINED)
      ));
      rdpq_mode_alphacompare(100);
      rdpq_mode_filter(FILTER_POINT);
    rdpq_mode_end();

  int heightPart = SCREEN_HEIGHT / SHADE_BLEND_SLICES;
  int y=0;
  int yEnd = y + heightPart;

  for(; y<yEnd; y+=3) {
    int nextY = y + 3;
    rdpq_tex_multi_begin();
      rdpq_tex_upload_sub(TILE0, &placeShade, nullptr, 0, y, SCREEN_WIDTH, nextY);
      rdpq_tex_upload_sub(TILE1, &placeTex, nullptr, 0, y, SCREEN_WIDTH, nextY);
    rdpq_tex_multi_end();
    rdpq_texture_rectangle(TILE0, 0, y, SCREEN_WIDTH, nextY, 0, y);
  }
  blendBlocks[0] = rspq_block_end();

  for(int part=1; part<SHADE_BLEND_SLICES; ++part) {
    yEnd += heightPart;
    rspq_block_begin();
    for(; y<yEnd; y+=3) {
        int nextY = y + 3;
        rdpq_tex_multi_begin();
          rdpq_tex_upload_sub(TILE0, &placeShade, nullptr, 0, y, SCREEN_WIDTH, nextY);
          rdpq_tex_upload_sub(TILE1, &placeTex, nullptr, 0, y, SCREEN_WIDTH, nextY);
        rdpq_tex_multi_end();
        rdpq_texture_rectangle(TILE0, 0, y, SCREEN_WIDTH, nextY, 0, y);
      }
    blendBlocks[part] = rspq_block_end();
  }
}

FbBlend::~FbBlend() {
  for(auto & blendBlock : blendBlocks) {
    rspq_block_free(blendBlock);
  }
}

