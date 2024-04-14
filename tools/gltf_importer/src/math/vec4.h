/**
* @copyright 2022 - Max Bebök
* @license MIT
*/

#pragma once

#include "vec3.h"

struct Vec4
{
  float data[4]{0.0f};

  // Constructors
  constexpr Vec4() = default;
  constexpr Vec4(float s): data{s,s,s,s} {};
  constexpr Vec4(float x, float y, float z, float w): data{x,y,z,w} {};
  constexpr Vec4(const Vec3 &vec, float w = 0.0f): data{vec[0], vec[1], vec[2], w} {};
  constexpr Vec4(const Vec4& v) = default;

  // conversion
  [[nodiscard]] const Vec3& toVec3() const {
    return *(const Vec3*)data;
  }
  [[nodiscard]] Vec3& toVec3() {
    return *(Vec3*)data;
  }

  // Data accessor
  float& operator[](u64 index) { return data[index]; }
  constexpr const float& operator[](u64 index) const { return data[index]; }

  [[nodiscard]] const float* ptr() const { return data; }
  float* ptr() { return data; }

  [[nodiscard]] float& x() { return data[0]; }
  [[nodiscard]] float& y() { return data[1]; }
  [[nodiscard]] float& z() { return data[2]; }
  [[nodiscard]] float& w() { return data[3]; }

  [[nodiscard]] const float& x() const { return data[0]; }
  [[nodiscard]] const float& y() const { return data[1]; }
  [[nodiscard]] const float& z() const { return data[2]; }
  [[nodiscard]] const float& w() const { return data[3]; }

  [[nodiscard]] float& r() { return data[0]; }
  [[nodiscard]] float& g() { return data[1]; }
  [[nodiscard]] float& b() { return data[2]; }
  [[nodiscard]] float& a() { return data[3]; }

  [[nodiscard]] const float& r() const { return data[0]; }
  [[nodiscard]] const float& g() const { return data[1]; }
  [[nodiscard]] const float& b() const { return data[2]; }
  [[nodiscard]] const float& a() const { return data[3]; }

  // Helper
  constexpr void clear() {
    data[0] = 0.0f; data[1] = 0.0f;
    data[2] = 0.0f; data[3] = 0.0f;
  }

  [[nodiscard]] int getLargestIdx() const {
    int idx = 0;
    for(int i = 1; i < 4; ++i) {
      if(data[i] > data[idx])idx = i;
    }
    return idx;
  }

  [[nodiscard]] float sum() const {
    return data[0] + data[1] + data[2] + data[3];
  }

  [[nodiscard]] float dot(const Vec4 a) const {
    return a[0] * data[0] +
           a[1] * data[1] +
           a[2] * data[2] +
           a[3] * data[3];
  }

  float length2() const {
    return dot(*this);
  }

  // Operations (Vector)
  constexpr Vec4 operator-() const {
    return { -data[0], -data[1], -data[2], -data[3] };
  }

  constexpr Vec4 operator+(const Vec4& b) const {
    return {
      data[0] + b[0], data[1] + b[1],
      data[2] + b[2], data[3] + b[3],
    };
  }

  constexpr Vec4 operator-(const Vec4& b) const {
    return {
      data[0] - b[0], data[1] - b[1],
      data[2] - b[2], data[3] - b[3],
    };
  }

  constexpr Vec4 operator*(const Vec4& b) const {
    return {
      data[0] * b[0], data[1] * b[1],
      data[2] * b[2], data[3] * b[3],
    };
  }

  constexpr Vec4 operator/(const Vec4& b) const {
    return {
      data[0] / b[0], data[1] / b[1],
      data[2] / b[2], data[3] / b[3],
    };
  }

  Vec4& operator+=(const Vec4& b) { return *this = *this + b; }
  Vec4& operator-=(const Vec4& b) { return *this = *this - b; }
  Vec4& operator*=(const Vec4& b) { return *this = *this * b; }
  Vec4& operator/=(const Vec4& b) { return *this = *this / b; }

  // Operations (Scalar)
  Vec4 operator+(float b) const {
    return {
      data[0] + b, data[1] + b,
      data[2] + b, data[3] + b,
    };
  }

  Vec4 operator-(float b) const {
    return {
      data[0] - b, data[1] - b,
      data[2] - b, data[3] - b,
    };
  }

  Vec4 operator*(float b) const {
    return {
      data[0] * b, data[1] * b,
      data[2] * b, data[3] * b,
    };
  }

  Vec4 operator/(float b) const {
    return {
      data[0] / b, data[1] / b,
      data[2] / b, data[3] / b,
    };
  }

  Vec4& operator+=(float b) { return *this = *this + b; }
  Vec4& operator-=(float b) { return *this = *this - b; }
  Vec4& operator*=(float b) { return *this = *this * b; }
  Vec4& operator/=(float b) { return *this = *this / b; }

  // Comparison
  bool operator==(const Vec4&) const = default;
};