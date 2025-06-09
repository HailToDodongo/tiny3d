#include "postProcess.h"
#include "rspFX.h"
#include <utility>

#define MEASURE_PERF 0

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

    ~TimedHighPrio(){
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

  surfHDR = surface_alloc(FMT_RGBA32, display_get_width(), display_get_height() + 2);
  surfBlurA = surface_alloc(FMT_RGBA32, surfHDR.width / SCALE_FACTOR, surfHDR.height / SCALE_FACTOR + 4);
  surfBlurB = surface_alloc(FMT_RGBA32, surfHDR.width / SCALE_FACTOR, surfHDR.height / SCALE_FACTOR + 4);

  surfHDRSafe = surface_make_sub(&surfHDR, 0, 1, surfHDR.width, surfHDR.height-1);
  surfBlurASafe = surface_make_sub(&surfBlurA, 0, 2, surfBlurA.width, surfBlurA.height-2);
  surfBlurBSafe = surface_make_sub(&surfBlurB, 0, 2, surfBlurB.width, surfBlurB.height-2);
}

PostProcess::~PostProcess()
{
  surface_free(&surfBlurA);
  surface_free(&surfBlurB);
}

void PostProcess::attachHDR()
{
  rdpq_set_color_image(&surfHDR);
}

surface_t& PostProcess::hdrBloom(surface_t& dst, const PostProcessConf &conf)
{
  surface_t *input = &surfBlurBSafe;
  surface_t *output = &surfBlurASafe;

  //rdpq_fence();
  //rspq_wait();

  { // First Pass, downscale image 4:1 with interpolation
    TimedHighPrio p{"RSP Scale"};
    RspFX::downscale(surfHDRSafe.buffer, output->buffer);
  }

  { // Now blur the smaller image N amount of times by ping-ponging the buffers
    TimedHighPrio p{"RSP Blur"};
    for(int i=0; i<conf.blurSteps; ++i) {
      std::swap(input, output);
      RspFX::blur(input->buffer, output->buffer, conf.blurBrightness);
    }
  }

  { // Combine original image and blurred image in a combined HDR+Bloom pass
    TimedHighPrio p{"RSP Bloom"};
    RspFX::hdrBlit(surfHDRSafe.buffer, dst.buffer, output->buffer, conf.hdrFactor);
  }

  return *output;
}