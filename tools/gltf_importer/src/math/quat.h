/**
* @copyright 2022 - Max Bebök
* @license MIT
*/
#pragma once

#include "../types.h"
#include "vec4.h"
#include <math.h>

struct Quat
{
  f32 data[4] = {0.0f, 0.0f, 0.0f, 1.0f};

  // Constructors
  constexpr Quat() = default;
  constexpr Quat(f32 s): data{s,s,s,s} {};
  constexpr Quat(f32 x, f32 y, f32 z, f32 w): data{x,y,z,w} {};
  constexpr Quat(const Vec4 &v): data{v[0],v[1],v[2],v[3]} {};
  constexpr Quat(const Quat& q) = default;

  Quat(const Vec3 &euler)
  {
    auto eulCosX = cosf(euler[0] * 0.5f);
    auto eulCosY = cosf(euler[1] * 0.5f);
    auto eulCosZ = cosf(euler[2] * 0.5f);

    auto eulSinX = sinf(euler[0] * 0.5f);
    auto eulSinY = sinf(euler[1] * 0.5f);
    auto eulSinZ = sinf(euler[2] * 0.5f);

    w() = eulCosX * eulCosY * eulCosZ + eulSinX * eulSinY * eulSinZ;
    x() = eulSinX * eulCosY * eulCosZ - eulCosX * eulSinY * eulSinZ;
    y() = eulCosX * eulSinY * eulCosZ + eulSinX * eulCosY * eulSinZ;
    z() = eulCosX * eulCosY * eulSinZ - eulSinX * eulSinY * eulCosZ;
  }

  [[nodiscard]] const f32* ptr() const { return data; }
  f32* ptr() { return data; }

  float& operator[](u64 index) { return data[index]; }
  constexpr const float& operator[](u64 index) const { return data[index]; }

  [[nodiscard]] f32& x() { return data[0]; }
  [[nodiscard]] f32& y() { return data[1]; }
  [[nodiscard]] f32& z() { return data[2]; }
  [[nodiscard]] f32& w() { return data[3]; }

  [[nodiscard]] const f32& x() const { return data[0]; }
  [[nodiscard]] const f32& y() const { return data[1]; }
  [[nodiscard]] const f32& z() const { return data[2]; }
  [[nodiscard]] const f32& w() const { return data[3]; }

  // Comparison
  bool operator==(const Quat&) const = default;

  // Helper
  void clear() {
    data[0] = 0.0f; data[1] = 0.0f;
    data[2] = 0.0f; data[3] = 0.0f;
  }

  [[nodiscard]] bool isIdentity() const {
    return data[0] == 0.0f && data[1] == 0.0f && data[2] == 0.0f && data[3] == 1.0f;
  }

  [[nodiscard]] Vec4 toVec4() const {
    return {data[0], data[1], data[2], data[3]};
  }

  [[nodiscard]] Quat slerp(const Quat &b, float factor) const {
    Quat res;
    float dot = x() * b.x() + y() * b.y() + z() * b.z() + w() * b.w();
    if(dot < 0) {
      return -(*this).slerp(-b, factor);
    }

    float theta = acosf(dot);

    if (fabsf(theta) < 0.0001f || isnan(theta) || isinf(theta)) {
      res = *this;
    } else {
      float sinTheta = sinf(theta);
      float s0 = sinf((1.0f - factor) * theta) / sinTheta;
      float s1 = sinf(factor * theta) / sinTheta;
      res.x() = s0 * x() + s1 * b.x();
      res.y() = s0 * y() + s1 * b.y();
      res.z() = s0 * z() + s1 * b.z();
      res.w() = s0 * w() + s1 * b.w();
    }
    return res;
  };

  // Operations
  Quat operator*(const Quat& b) const
  {
    return {
      data[3] * b.data[0] + data[0] * b.data[3] + data[1] * b.data[2] - data[2] * b.data[1],
      data[3] * b.data[1] - data[0] * b.data[2] + data[1] * b.data[3] + data[2] * b.data[0],
      data[3] * b.data[2] + data[0] * b.data[1] - data[1] * b.data[0] + data[2] * b.data[3],
      data[3] * b.data[3] - data[0] * b.data[0] - data[1] * b.data[1] - data[2] * b.data[2]
    };
  }

  Quat operator-() const
  {
    return { -data[0], -data[1], -data[2], -data[3] };
  }

  Quat inverse() const
  {
    float normSquared = data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3];
    normSquared = 1.0f / normSquared;

    return { -data[0] * normSquared, -data[1] * normSquared, -data[2] * normSquared, data[3] * normSquared };
  }

  bool isInvalid() const {
    return isnan(data[0]) || isnan(data[1]) || isnan(data[2]) || isnan(data[3])
    || isinf(data[0]) || isinf(data[1]) || isinf(data[2]) || isinf(data[3]);
  }

  std::string toString() const {
    return std::string{"("} + std::to_string(data[0]) + ", " + std::to_string(data[1]) + ", " + std::to_string(data[2]) + ", " + std::to_string(data[3]) + ")";
  }
};