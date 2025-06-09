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
    surface_t surfBlurA{};
    surface_t surfBlurB{};

    // subsection of the above to allow OOB access in the ucode
    surface_t surfHDRSafe{};
    surface_t surfBlurASafe{};
    surface_t surfBlurBSafe{};


  public:
    PostProcess();
    ~PostProcess();

    void attachHDR();
    surface_t &hdrBloom(surface_t& dst, const PostProcessConf &conf);
};
