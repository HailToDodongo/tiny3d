/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "rspFX.h"
#include <libdragon.h>

extern "C" {
  DEFINE_RSP_UCODE(rsp_fx);
}

namespace {
  uint32_t rspIdFX{0};
}

void RSP::FX::init()
{
  if(rspIdFX == 0) {
    rspIdFX = rspq_overlay_register(&rsp_fx);
  }
}

void RSP::FX::fillTextures(uint32_t fbTex, uint32_t fbTexEnd, uint32_t fbOut)
{
  RSP::write(rspIdFX, 0,
    (uint32_t)fbTex    & 0xFFFFFF,
    (uint32_t)fbTexEnd & 0xFFFFFF,
    (uint32_t)fbOut    & 0xFFFFFF
  );
}
