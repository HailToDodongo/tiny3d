/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

struct PostProcessConf {
  int blurSteps{};
  float blurBrightness{};
  float hdrFactor{};
};

class PostProcess
{
  private:
    surface_t surfHDR{};
    rspq_block_t *blendBlock{};
    surface_t surfBlurA{};
    surface_t surfBlurB{};

  public:
    PostProcess();
    ~PostProcess();

    void attachHDR();
    surface_t &hdrBloom(surface_t& dst, const PostProcessConf &conf);
};
