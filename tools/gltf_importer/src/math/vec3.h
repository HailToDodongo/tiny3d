/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#pragma once

#include "../types.h"
#include <cmath>

struct Vec3
{
  f32 data[3]{0.0f};

  // Constructors
  constexpr Vec3() = default;
  constexpr Vec3(f32 s): data{s,s,s} {};
  constexpr Vec3(f32 x, f32 y, f32 z): data{x,y,z} {};
  constexpr Vec3(const Vec3& v) = default;

  static constexpr Vec3 UP() { return {0.0f, 1.0f, 0.0f}; }
  static constexpr Vec3 DOWN() { return {0.0f, -1.0f, 0.0f}; }

  static constexpr Vec3 fromS8(const s8 (&values)[3]) {
    return Vec3{
      static_cast<f32>(values[0]) / 127.0f,
      static_cast<f32>(values[1]) / 127.0f,
      static_cast<f32>(values[2]) / 127.0f
    };
  }

  // Data accessor
  f32& operator[](u32 index) { return data[index]; }
  constexpr const f32& operator[](u32 index) const { return data[index]; }

  [[nodiscard]] const f32* ptr() const { return data; }
  f32* ptr() { return data; }

  [[nodiscard]] f32& x() { return data[0]; }
  [[nodiscard]] f32& y() { return data[1]; }
  [[nodiscard]] f32& z() { return data[2]; }

  [[nodiscard]] const f32& x() const { return data[0]; }
  [[nodiscard]] const f32& y() const { return data[1]; }
  [[nodiscard]] const f32& z() const { return data[2]; }

  // Helper
  void clear() {
    data[0] = 0.0f; data[1] = 0.0f;
    data[2] = 0.0f;
  }

  [[nodiscard]] f32 dot(const Vec3 a) const {
    return a[0] * data[0] + a[1] * data[1] + a[2] * data[2];
  }

  [[nodiscard]] Vec3 cross(const Vec3 &y) const {
    return {
        data[1]*y[2] - y[1]*data[2],
        data[2]*y[0] - y[2]*data[0],
        data[0]*y[1] - y[0]*data[1],
    };
  }

  [[nodiscard]] Vec3 round() const {
    return {
      ::roundf(data[0]),
      ::roundf(data[1]),
      ::roundf(data[2])
    };
  }

  [[nodiscard]] Vec3 normalize() const {
    f32 len = sqrtf(data[0]*data[0] + data[1]*data[1] + data[2]*data[2]);
    return *this / len;
  }

  [[nodiscard]] constexpr Vec3 clamp(f32 min, f32 max) const {
    return {
      data[0] < min ? min : (data[0] > max ? max : data[0]),
      data[1] < min ? min : (data[1] > max ? max : data[1]),
      data[2] < min ? min : (data[2] > max ? max : data[2])
    };
  }

  [[nodiscard]] constexpr Vec3 clamp(const Vec3 &vMin, const Vec3 &vMax) const {
    return {
      data[0] < vMin[0] ? vMin[0] : (data[0] > vMax[0] ? vMax[0] : data[0]),
      data[1] < vMin[1] ? vMin[1] : (data[1] > vMax[1] ? vMax[1] : data[1]),
      data[2] < vMin[2] ? vMin[2] : (data[2] > vMax[2] ? vMax[2] : data[2])
    };
  }

  [[nodiscard]] constexpr Vec3 mix(const Vec3 &b, float factor) const {
    return {
      data[0] + (b[0] - data[0]) * factor,
      data[1] + (b[1] - data[1]) * factor,
      data[2] + (b[2] - data[2]) * factor
    };
  }

  [[nodiscard]] constexpr bool isZero() const {
    return data[0] == 0.0f && data[1] == 0.0f && data[2] == 0.0f;
  }

  // Operations (Vector)
  constexpr Vec3 operator-() const {
    return { -data[0], -data[1], -data[2] };
  }

  constexpr Vec3 operator+(const Vec3& b) const {
    return {
      data[0] + b[0], data[1] + b[1],
      data[2] + b[2]
    };
  }

  constexpr Vec3 operator-(const Vec3& b) const {
    return {
      data[0] - b[0], data[1] - b[1],
      data[2] - b[2]
    };
  }

  constexpr Vec3 operator*(const Vec3& b) const {
    return {
      data[0] * b[0], data[1] * b[1],
      data[2] * b[2]
    };
  }

  constexpr Vec3 operator/(const Vec3& b) const {
    return {
      data[0] / b[0], data[1] / b[1],
      data[2] / b[2]
    };
  }

  Vec3& operator+=(const Vec3& b) { return *this = *this + b; }
  Vec3& operator-=(const Vec3& b) { return *this = *this - b; }
  Vec3& operator*=(const Vec3& b) { return *this = *this * b; }
  Vec3& operator/=(const Vec3& b) { return *this = *this / b; }

  // Operations (Scalar)
  Vec3 operator+(float b) const {
    return {
      data[0] + b, data[1] + b,
      data[2] + b
    };
  }

  Vec3 operator-(float b) const {
    return {
      data[0] - b, data[1] - b,
      data[2] - b
    };
  }

  Vec3 operator*(float b) const {
    return {
      data[0] * b, data[1] * b,
      data[2] * b
    };
  }

  Vec3 operator/(float b) const {
    return {
      data[0] / b, data[1] / b,
      data[2] / b
    };
  }

  Vec3& operator+=(float b) { return *this = *this + b; }
  Vec3& operator-=(float b) { return *this = *this - b; }
  Vec3& operator*=(float b) { return *this = *this * b; }
  Vec3& operator/=(float b) { return *this = *this / b; }

  // Comparison
  bool operator==(const Vec3&) const = default;
};
