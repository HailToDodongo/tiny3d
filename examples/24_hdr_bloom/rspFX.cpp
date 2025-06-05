/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "rspFX.h"
#include <libdragon.h>

extern "C" {
  DEFINE_RSP_UCODE(rsp_fx);

  inline void rspq_write_2(uint32_t rspID, uint32_t cmd, uint32_t a, uint32_t b)
  {
    rspq_write(rspID, cmd, a, b);
  }

  inline void rspq_write_3(uint32_t rspID, uint32_t cmd, uint32_t a, uint32_t b, uint32_t c)
  {
    rspq_write(rspID, cmd, a, b, c);
  }
}

namespace {
  uint32_t rspIdFX{0};
}

void RspFX::init()
{
  if(rspIdFX == 0) {
    rspIdFX = rspq_overlay_register(&rsp_fx);
  }
}

void RspFX::hdrBlit(void* rgba32In, void *rgba16Out, float factor)
{
  uint32_t factorInt = (uint32_t)(factor * 64) & 0xFFFF;
  rspq_write_3(rspIdFX, 0,
     (uint32_t)rgba32In & 0xFFFFFF,
     (uint32_t)rgba16Out & 0xFFFFFF,
     factorInt
  );
}
