/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once

namespace RDP::SOM {
  constexpr uint64_t ALPHA_COMPARE = 1 << 0;
  constexpr uint64_t ALPHA_COMPARE_MASK = 1 << 0;

  constexpr uint64_t ZMODE_OPAQUE   = 0 << 10;
  constexpr uint64_t ZMODE_INTERPEN = 1 << 10;
  constexpr uint64_t ZMODE_TRANSP   = 2 << 10;
  constexpr uint64_t ZMODE_DECAL    = 3 << 10;
  constexpr uint64_t ZMODE_MASK     = 3 << 10;

  constexpr uint64_t SAMPLE_SHIFT = 44;
  constexpr uint64_t SAMPLE_POINT    = (uint64_t)0 << SAMPLE_SHIFT;
  constexpr uint64_t SAMPLE_BILINEAR = (uint64_t)2 << SAMPLE_SHIFT;
  constexpr uint64_t SAMPLE_MEDIAN   = (uint64_t)3 << SAMPLE_SHIFT;
  constexpr uint64_t SAMPLE_MASK     = (uint64_t)3 << SAMPLE_SHIFT;

  constexpr uint64_t BLALPHA_MASK     = 3 << 12;
  constexpr uint64_t BLALPHA_CC       = 0 << 12;
  constexpr uint64_t BLALPHA_CVG      = 2 << 12;
  constexpr uint64_t BLALPHA_CVG_X_CC = 1 << 12;
}

namespace RDP::BLEND {
  constexpr uint32_t NONE      = 0x0000'0000;
  constexpr uint32_t MULTIPLY  = 0x0050'0040;
  constexpr uint32_t MUL_CONST = 0x0550'0040;
  constexpr uint32_t ADDITIVE  = 0x005A'0040;
}