/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once

// Note: compat for gcc-11 which doesn't have std::byteswap() yet
#include <cstring>

namespace Bit
{
  constexpr uint8_t byteswap(uint8_t val) { return val; }
  constexpr int8_t byteswap(int8_t val) { return val; }

  constexpr uint16_t byteswap(uint16_t val) {
     return ((val >> (8*1)) & 0x00FF) |
            ((val << (8*1)) & 0xFF00);
  }
  constexpr int16_t byteswap(int16_t val) {
    return (int16_t)byteswap((uint16_t)val);
  }

  constexpr uint32_t byteswap(uint32_t val) {
     return ((val >> (8*3)) & 0x0000'00FF) |
            ((val >> (8*1)) & 0x0000'FF00) |
            ((val << (8*1)) & 0x00FF'0000) |
            ((val << (8*3)) & 0xFF00'0000);
  }

  constexpr int32_t byteswap(int32_t val) {
    return (int32_t)byteswap((uint32_t)val);
  }

  constexpr uint64_t byteswap(uint64_t val) {
     return ((val >> (8*7)) & 0x0000'0000'0000'00FF) |
            ((val >> (8*5)) & 0x0000'0000'0000'FF00) |
            ((val >> (8*3)) & 0x0000'0000'00FF'0000) |
            ((val >> (8*1)) & 0x0000'0000'FF00'0000) |
            ((val << (8*1)) & 0x0000'00FF'0000'0000) |
            ((val << (8*3)) & 0x0000'FF00'0000'0000) |
            ((val << (8*5)) & 0x00FF'0000'0000'0000) |
            ((val << (8*7)) & 0xFF00'0000'0000'0000);
  }

  constexpr int64_t byteswap(int64_t val) {
    return (int64_t)byteswap((uint64_t)val);
  }

  template <class OUT, class IN>
  OUT bit_cast(IN val) {
    static_assert(sizeof(IN) == sizeof(OUT), "Non matching size");
    OUT res;
    std::memcpy(std::addressof(res), std::addressof(val), sizeof(IN));
    return res;
  }
}