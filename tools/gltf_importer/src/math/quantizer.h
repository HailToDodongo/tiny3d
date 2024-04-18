/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once

namespace Quantizer
{
  constexpr float SQRT_2_INV = 0.70710678118f;

  // Quantize a float by simply remapping it to a normalized 16bit integer
  inline uint16_t floatToU16(float value, float offset, float scale)
  {
    return (uint16_t)round((double)(value - offset) / scale * 65535.0);
  }

  void floatsGetOffsetScale(const std::vector<Keyframe> &keyframes, float &offset, float &scale) {
    float valMin = INFINITY;
    float valMax = -INFINITY;
    for(auto &kf : keyframes) {
      valMin = std::min(valMin, kf.valScalar);
      valMax = std::max(valMax, kf.valScalar);
    }
    offset = valMin;
    scale = valMax - valMin;
  }

  inline std::vector<uint16_t> floatsToU16(const std::vector<float> &values, float offset, float scale) {
    std::vector<uint16_t> res;
    for(float val : values) {
      res.push_back(Quantizer::floatToU16(val, offset, scale));
    }
    return res;
  }

  // Quantize a float by simply remapping it to a normalized 10bit integer (stored as u32)
  // Average error: 0.000346 in range -SQRT_2_INV to SQRT_2_INV
  inline uint32_t floatToU10(float value, float offset, float scale) {
    return (uint32_t)round((double)(value - offset) / scale * 1023.0);
  }

  inline float u10ToFloat(uint32_t value, float offset, float scale) {
    return (float)value / 1023.0 * scale + offset;
  }

  /**
   * Quantizes a quaternion into a 32bit integer.
   * The smallest 3 components are stored and given 10 bits,
   * with the 2 MSB being the index of the largest omitted component.
   */
  inline uint32_t quatTo32Bit(const Quat &q)
  {
    constexpr float rangeMin = -SQRT_2_INV;
    constexpr float rangeScale = SQRT_2_INV + SQRT_2_INV;

    auto qSq = q.toVec4() * q.toVec4();
    int largestIdx = qSq.getLargestIdx();
    float valNeg = q[largestIdx] >= 0 ? 1.0f : -1.0f;

    int idx0 = (largestIdx + 1) % 4;
    int idx1 = (largestIdx + 2) % 4;
    int idx2 = (largestIdx + 3) % 4;

    uint32_t q0 = floatToU10(valNeg * q[idx0], rangeMin, rangeScale);
    uint32_t q1 = floatToU10(valNeg * q[idx1], rangeMin, rangeScale);
    uint32_t q2 = floatToU10(valNeg * q[idx2], rangeMin, rangeScale);

    return (largestIdx << 30) | (q0 << 20) | (q1 << 10) | q2;
  }
}