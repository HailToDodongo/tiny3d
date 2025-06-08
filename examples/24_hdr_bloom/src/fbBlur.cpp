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

  void downscaleRSP(surface_t &src, surface_t &dst)
  {
    rspq_wait();
    auto t = get_ticks();
    rspq_highpri_begin();
    RspFX::downscale(src.buffer, dst.buffer);
    rspq_highpri_end();
    rspq_flush();
    rspq_highpri_sync();
    t = get_ticks() - t;
    debugf("RSP Scale: %.4fms\n", (double)TICKS_TO_US(t) / 1000.0f);
  }

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
    debugf("RSP Blur: %.4fms\n", (double)TICKS_TO_US(t) / 1000.0f);
  }
}

surface_t& FbBlur::blur(surface_t& src)
{
  downscaleRSP(src, surfBlurA);

  auto held  = joypad_get_buttons_held(JOYPAD_PORT_1);
  if(held.a)return surfBlurA;

  blurRSP(surfBlurA, surfBlurB);
  return surfBlurB;
}