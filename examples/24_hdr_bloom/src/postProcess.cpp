#include "postProcess.h"
#include "rspFX.h"
#include <utility>

namespace {
  constexpr int SCREEN_WIDTH = 320;
  constexpr int SCREEN_HEIGHT = 240;
  constexpr int SCALE_FACTOR = 4;

  struct TimedHighPrio {
    const char* name{};
    uint64_t t{};

    TimedHighPrio(const char* name): name{name}, t(get_ticks()) {
      rspq_highpri_begin();
    }

    ~TimedHighPrio(){
      rspq_highpri_end();
      rspq_flush();
      rspq_highpri_sync();
      t = get_ticks() - t;
      debugf("%s: %.4fms\n", name, (double)TICKS_TO_US(t) / 1000.0f);
    }
  };
}

PostProcess::PostProcess()
{
  assertf(display_get_width() == SCREEN_WIDTH, "Ucode can only handle 320x240 resolution");
  assertf(display_get_height() == SCREEN_HEIGHT, "Ucode can only handle 320x240 resolution");

  surfHDR = surface_alloc(FMT_RGBA32, display_get_width(), display_get_height());
  // slightly over allocate so the ucode can safely read/write go OOB
  surfBlurA = surface_alloc(FMT_RGBA32, surfHDR.width / SCALE_FACTOR, surfHDR.height / SCALE_FACTOR + 2);
  surfBlurB = surface_alloc(FMT_RGBA32, surfHDR.width / SCALE_FACTOR, surfHDR.height / SCALE_FACTOR + 2);
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
  surface_t *input = &surfBlurB;
  surface_t *output = &surfBlurA;

  //rdpq_fence();
  rspq_wait();

  { // First Pass, downscale image 4:1 with interpolation
    TimedHighPrio p{"RSP Scale"};
    RspFX::downscale(surfHDR.buffer, output->buffer);
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
    RspFX::hdrBlit(surfHDR.buffer, dst.buffer, output->buffer, conf.hdrFactor);
  }

  return *output;
}