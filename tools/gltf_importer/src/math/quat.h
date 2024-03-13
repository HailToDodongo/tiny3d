/**
* @copyright 2022 - Max Bebök
* @license MIT
*/
#pragma once

#include "../types.h"
#include <math.h>

struct Quat
{
  f32 data[4] = {1.0f, 0.0f, 0.0f, 0.0f};

  // Constructors
  constexpr Quat() = default;
  constexpr Quat(f32 s): data{s,s,s,s} {};
  constexpr Quat(f32 w, f32 x, f32 y, f32 z): data{w,x,y,z} {};
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

  [[nodiscard]] f32& x() { return data[1]; }
  [[nodiscard]] f32& y() { return data[2]; }
  [[nodiscard]] f32& z() { return data[3]; }
  [[nodiscard]] f32& w() { return data[0]; }

  [[nodiscard]] const f32& x() const { return data[1]; }
  [[nodiscard]] const f32& y() const { return data[2]; }
  [[nodiscard]] const f32& z() const { return data[3]; }
  [[nodiscard]] const f32& w() const { return data[0]; }

  // Comparison
  bool operator==(const Quat&) const = default;

  // Helper
  void clear() {
    data[0] = 0.0f; data[1] = 0.0f;
    data[2] = 0.0f; data[3] = 0.0f;
  }

  // Operations
  Quat operator*(const Quat& b) const
  {
    return {
      w() * b.w() - x() * b.x() - y() * b.y() - z() * b.z(),
      w() * b.x() + x() * b.w() + y() * b.z() - z() * b.y(),
      w() * b.y() + y() * b.w() + z() * b.x() - x() * b.z(),
      w() * b.z() + z() * b.w() + x() * b.y() - y() * b.x(),
    };
  }
};