/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include <t3d/t3dmath.h>

extern "C" {
  extern volatile uint32_t *rspq_cur_pointer, *rspq_cur_sentinel;
  extern void rspq_next_buffer(void);
}

namespace RSP {
  template<typename T0 = uint32_t, std::integral ...T>
  void write(uint32_t overlayId, uint32_t commandId, T0 arg0 = 0, T...args)
  {
    volatile uint32_t *ptr = rspq_cur_pointer + 1;
    ((*ptr = args, ++ptr), ...);

    *rspq_cur_pointer = arg0 | overlayId | ((commandId) << 24);
    rspq_cur_pointer += sizeof...(args) + 1;

    if (__builtin_expect(rspq_cur_pointer > rspq_cur_sentinel, 0)) {
       rspq_next_buffer();
    }
  }
}

namespace RSP::FX
{
  void init();
  void fillTextures(uint32_t fbTex, uint32_t fbTexEnd, uint32_t fbOut);
}
