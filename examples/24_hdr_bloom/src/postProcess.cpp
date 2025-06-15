#include "postProcess.h"
#include "rsp/rspFX.h"
#include <utility>

#define MEASURE_PERF 1

namespace {
  constexpr int SCREEN_WIDTH = 320;
  constexpr int SCREEN_HEIGHT = 240;
  constexpr int SCALE_FACTOR = 4;

  struct TimedHighPrio {
    const char* name{};
    uint64_t t{};

    TimedHighPrio(const char* name): name{name}, t(get_ticks()) {
      #if MEASURE_PERF
        rspq_highpri_begin();
      #endif
    }

    ~TimedHighPrio() {
      #if MEASURE_PERF
        rspq_highpri_end();
        rspq_flush();
        rspq_highpri_sync();
        t = get_ticks() - t;
        debugf("%s: %.4fms\n", name, (double)TICKS_TO_US(t) / 1000.0f);
      #endif
    }
  };
}

PostProcess::PostProcess()
{
  assertf(display_get_width() == SCREEN_WIDTH, "Ucode can only handle 320x240 resolution");
  assertf(display_get_height() == SCREEN_HEIGHT, "Ucode can only handle 320x240 resolution");

  int sizeLowX = display_get_width() / SCALE_FACTOR;
  int sizeLowY = display_get_height() / SCALE_FACTOR;

  surfHDR = surface_alloc(FMT_RGBA32, display_get_width(), display_get_height() + 4);
  surfBlurA = surface_alloc(FMT_RGBA32, sizeLowX, sizeLowY + 4);
  surfBlurB = surface_alloc(FMT_RGBA32, sizeLowX, sizeLowY + 4);

  surfHDRSafe = surface_make_sub(&surfHDR, 0, 2, surfHDR.width, display_get_height());
  surfBlurASafe = surface_make_sub(&surfBlurA, 0, 2, surfBlurA.width, sizeLowY);
  surfBlurBSafe = surface_make_sub(&surfBlurB, 0, 2, surfBlurB.width, sizeLowY);
}

PostProcess::~PostProcess()
{
  surface_free(&surfBlurB);
  surface_free(&surfBlurA);
  surface_free(&surfHDR);
  if(blockRDPScale)rspq_block_free(blockRDPScale);
}

void PostProcess::beginFrame()
{
  rdpq_set_color_image(&surfHDRSafe);
}

void PostProcess::endFrame()
{
  if(!conf.scalingUseRDP)return;

  if(!blockRDPScale)
  {
    rspq_block_begin();
    rdpq_sync_pipe();
    rdpq_sync_load();
    rdpq_set_mode_standard();

    rdpq_mode_begin();
      rdpq_mode_filter(FILTER_MEDIAN);
      rdpq_mode_antialias(AA_NONE);
      rdpq_mode_dithering(DITHER_NONE_NONE);
      rdpq_mode_blender(0);

      rdpq_mode_combiner(RDPQ_COMBINER2(
        (TEX0,TEX1,PRIM_ALPHA,TEX1), (0,0,0,1),
        (0,0,0,COMBINED),            (0,0,0,1)
      ));
    rdpq_mode_end();

    rdpq_texparms_t texParam0{};
    texParam0.s.scale_log = -2;
    texParam0.s.translate = 1.5f;
    texParam0.t.translate = 0.5f;
    auto texParam1 = texParam0;
    texParam1.s.translate += 2.0f;

    rdpq_set_prim_color({0,0,0, 0x100/2});
    rdpq_set_color_image(&surfBlurASafe);
    for(int y=0; y<surfBlurASafe.height; ++y)
    {
      auto surfSub = surface_make_sub(&surfHDRSafe, 0, y*4, surfHDRSafe.width, 2);
      surfSub.stride *= 2; // load every other line

      rdpq_tex_multi_begin();
        rdpq_tex_upload(TILE0, &surfSub, &texParam0);
        rdpq_tex_reuse(TILE1, &texParam1);
      rdpq_tex_multi_end();

      rdpq_texture_rectangle(TILE0, 0, y, surfBlurASafe.width, y+1, 1, 1);
    }
    blockRDPScale = rspq_block_end();
  }

  rspq_block_run(blockRDPScale);
}

surface_t& PostProcess::applyEffects(surface_t &dst)
{
  #ifdef MEASURE_PERF
    rspq_wait();
  #endif

  surface_t *input = &surfBlurBSafe;
  surface_t *output = &surfBlurASafe;

  float bloomFactor = conf.hdrFactor * 0.5f * conf.blurBrightness;

  int blurSteps = conf.blurSteps;
  if(blurSteps > 0 && bloomFactor <= 0.0f) {
    blurSteps = 1;
  }

  if(!conf.scalingUseRDP)
  { // First Pass, downscale image 4:1 with interpolation
    TimedHighPrio p{"RSP Scale"};
    RspFX::downscale(surfHDRSafe.buffer, output->buffer);
  }

  { // Now blur the smaller image N amount of times by ping-ponging the buffers
    TimedHighPrio p{"RSP Blur"};
    for(int i=0; i<blurSteps; ++i) {
      std::swap(input, output);
      RspFX::blur(
        input->buffer, output->buffer,
        (i == blurSteps-1) ? bloomFactor : 1.0f,
        (i == 0) ? conf.bloomThreshold : 0.0f
      );
    }
  }

  { // Combine original image and blurred image in a combined HDR+Bloom pass
    TimedHighPrio p{"RSP Bloom"};
    RspFX::hdrBlit(surfHDRSafe.buffer, dst.buffer, output->buffer, conf.hdrFactor);
  }

  // Read back image brightness, this is not synced here since we can live with a delay
  uint32_t *imgBrightness = (uint32_t*)(((char*)surfHDRSafe.buffer) + surfHDRSafe.stride * (surfHDRSafe.height));
  relBrightness = (double)(*imgBrightness >> 16) / (double)0x707E;

  return *output;
}
