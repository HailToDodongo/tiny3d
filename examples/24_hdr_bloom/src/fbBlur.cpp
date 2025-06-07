#include "fbBlur.h"
#include "rspFX.h"

FbBlur::FbBlur()
{
  surfBlurA = surface_alloc(FMT_RGBA32, 80, 60);
  surfBlurB = surface_alloc(FMT_RGBA32, 80, 60);
}

FbBlur::~FbBlur()
{

}

namespace {
  void blurRSP(surface_t &src, surface_t &dst)
  {
    rspq_wait();
    auto t = get_ticks();
    rspq_highpri_begin();
    RspFX::blur(src.buffer, dst.buffer, 1.0f);
    rspq_highpri_end();
    rspq_flush();
    rspq_highpri_sync();
    t = get_ticks() - t;
    debugf("RSP Time: %lldus\n", TICKS_TO_US(t));
  }
}

surface_t& FbBlur::blur(surface_t& src)
{
  rdpq_set_color_image(&surfBlurA);

  rdpq_set_mode_standard();
  rdpq_mode_begin();
    rdpq_mode_antialias(AA_NONE);
    rdpq_mode_blender(0);
  rdpq_mode_dithering(DITHER_BAYER_BAYER);
    rdpq_mode_combiner(RDPQ_COMBINER2(
      (TEX0,TEX1,PRIM_ALPHA,TEX1),       (0,0,0,1),
      (0,0,0,COMBINED),       (0,0,0,COMBINED)
    ));
  rdpq_mode_end();

  rdpq_mode_filter(FILTER_BILINEAR);

  rdpq_set_prim_color({0,0,0, 0xFF/2});

  /**
   * Sampling is setup to do 3 interpolations per final pixel.
   * A slice of the original buffer is uploaded, with 2 tiles using that data.
   * Each tile is offset by half a pixel, with the CC setup to LERP between them.
   *
   * So looking at a 3x3 pixel section, where each "[-]" is a pixel
   * [P]
   *
   *      [0]   [-]   [-]
   *                A
   *      [-]   [-]   [-]
   *          B
   *      [-]   [-]   [-]
   *
   * "P" is the origin defined in the tex-rect command set to (-1,-1)
   * Each tile then offsets to be between pixel.
   * The CC then LERPS those to effectively sample the center diagonally.
   */
  rdpq_texparms_t paramA{};
  paramA.s.translate = 1.5f;
  paramA.t.translate = 0.5f;

  rdpq_texparms_t paramB{};
  paramB.s.translate = 0.5f;
  paramB.t.translate = 1.5f;

  int outPosY = 1;
  for(int y=0; y<240; y+=12)
  {
    int outPosYEnd = outPosY + 12/4;
    int outPosX = 0;
    for(int x=0; x<320; x+=70)
    {
      int outPosXEnd = outPosX + 70/4;

      int uvSizeX = x == 280 ? 42 : 72;

      auto surfSub = surface_make_sub(&src,
        x==0 ? 0 : x-2,
        y==0 ? 0 : y-1,
        uvSizeX,
        y==(240-12) ? 12 : 14
      );
      rdpq_tex_multi_begin();
        rdpq_tex_upload(TILE0, &surfSub, &paramA);
        rdpq_tex_reuse(TILE1, &paramB);
      rdpq_tex_multi_end();

      // we are doing a downscale by 3, however to get a bit smaller and keep the math simpler do 4 here
      constexpr int UV_OFFSET = -1;
      rdpq_texture_rectangle_scaled(TILE0,
        outPosX, outPosY, outPosXEnd, outPosYEnd,
        0+UV_OFFSET, 0+UV_OFFSET,
        (70)+UV_OFFSET, 12+UV_OFFSET
      );

      outPosX = outPosXEnd;
    }
    outPosY = outPosYEnd;
  }

  blurRSP(surfBlurA, surfBlurB);
  return surfBlurB;
}