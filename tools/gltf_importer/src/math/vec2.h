/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#pragma once

#include "../types.h"
#include <cmath>

struct Vec2
{
  f32 data[2]{0.0f};

  // Constructors
  constexpr Vec2() = default;
  constexpr Vec2(f32 s): data{s,s} {};
  constexpr Vec2(f32 x, f32 y): data{x,y} {};
  constexpr Vec2(const Vec2& v) = default;

  // Data accessor
  f32& operator[](u32 index) { return data[index]; }
  constexpr const f32& operator[](u32 index) const { return data[index]; }

  [[nodiscard]] const f32* ptr() const { return data; }
  f32* ptr() { return data; }

  [[nodiscard]] f32& x() { return data[0]; }
  [[nodiscard]] f32& y() { return data[1]; }

  [[nodiscard]] const f32& x() const { return data[0]; }
  [[nodiscard]] const f32& y() const { return data[1]; }

  // Helper
  void clear() {
    data[0] = 0.0f;
    data[1] = 0.0f;
  }

  [[nodiscard]] constexpr bool isZero() const {
    return data[0] == 0.0f && data[1] == 0.0f;
  }

  [[nodiscard]] f32 dot(const Vec2 a) const {
    return a[0] * data[0] + a[1] * data[1];
  }

  [[nodiscard]] Vec2 round() const {
    return {
      ::roundf(data[0]),
      ::roundf(data[1]),
    };
  }

  // Operations (Vector)
  constexpr Vec2 operator-() const {
    return { -data[0], -data[1] };
  }

  constexpr Vec2 operator+(const Vec2& b) const {
    return { data[0] + b[0], data[1] + b[1] };
  }

  constexpr Vec2 operator-(const Vec2& b) const {
    return { data[0] - b[0], data[1] - b[1] };
  }

  constexpr Vec2 operator*(const Vec2& b) const {
    return { data[0] * b[0], data[1] * b[1] };
  }

  constexpr Vec2 operator/(const Vec2& b) const {
    return { data[0] / b[0], data[1] / b[1] };
  }

  Vec2& operator+=(const Vec2& b) { return *this = *this + b; }
  Vec2& operator-=(const Vec2& b) { return *this = *this - b; }
  Vec2& operator*=(const Vec2& b) { return *this = *this * b; }
  Vec2& operator/=(const Vec2& b) { return *this = *this / b; }

  // Operations (Scalar)
  Vec2 operator+(float b) const {
    return { data[0] + b, data[1] + b };
  }

  Vec2 operator-(float b) const {
    return { data[0] - b, data[1] - b };
  }

  Vec2 operator*(float b) const {
    return { data[0] * b, data[1] * b };
  }

  Vec2 operator/(float b) const {
    return { data[0] / b, data[1] / b };
  }

  Vec2& operator+=(float b) { return *this = *this + b; }
  Vec2& operator-=(float b) { return *this = *this - b; }
  Vec2& operator*=(float b) { return *this = *this * b; }
  Vec2& operator/=(float b) { return *this = *this / b; }

  // Comparison
  bool operator==(const Vec2&) const = default;
};
