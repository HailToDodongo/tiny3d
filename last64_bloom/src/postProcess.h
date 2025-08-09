/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

struct PostProcessConf {
  int blurSteps{}; // how often to blur the low-res image
  float blurBrightness{}; // brightness of the blur aka bloom
  float hdrFactor{}; // HDR exposure factor, 1.0 to get standard color range
  float bloomThreshold{}; // threshold to ignore pixels before blurring
  bool scalingUseRDP{}; // if true, use RDP for initial downscaling
};

class PostProcess
{
  private:
    surface_t surfHDR{};
    surface_t surfBlurA{};
    surface_t surfBlurB{};

    // subsection of the above to allow OOB access in the ucode
    surface_t surfHDRSafe{};
    surface_t surfBlurASafe{};
    surface_t surfBlurBSafe{};
    rspq_block_t *blockRDPScale{nullptr};

    PostProcessConf conf{};
    float relBrightness{0.0f};

  public:
    PostProcess();
    ~PostProcess();

    void setConf(const PostProcessConf &config) { conf = config; }

    void beginFrame();
    void endFrame();
    surface_t &applyEffects(surface_t& dst);

    float getBrightness() const { return relBrightness; }
};
