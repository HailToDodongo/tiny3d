/**
* @copyright 2022 - Max Bebök
* @license MIT
*/
#pragma once

#include "../types.h"
#include "vec3.h"
#include "quat.h"

struct Mat4
{
  f32 data[4][4]{
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
  };

  constexpr Mat4() = default;
  constexpr Mat4(const Mat4& v) = default;
  constexpr explicit Mat4(const float (&m)[4][4]) : data{
    {m[0][0], m[0][1], m[0][2], m[0][3]},
    {m[1][0], m[1][1], m[1][2], m[1][3]},
    {m[2][0], m[2][1], m[2][2], m[2][3]},
    {m[3][0], m[3][1], m[3][2], m[3][3]},
  } {}

  [[nodiscard]] const f32* ptr() const { return &data[0][0]; }

  Vec3& getPos() { return *(Vec3*)data[3]; }
  [[nodiscard]] const Vec3& getPos() const { return *(Vec3*)data[3]; }

  void setPos(const Vec3& newPos) {
    data[3][0] = newPos[0];
    data[3][1] = newPos[1];
    data[3][2] = newPos[2];
  }

  void setScale(const Vec3& scale) {
    data[0][0] = scale[0];
    data[1][1] = scale[1];
    data[2][2] = scale[2];
  }

  void setRot(const Quat& q) {
      f32 qxx = q.x() * q.x();
      f32 qyy = q.y() * q.y();
      f32 qzz = q.z() * q.z();
      f32 qxz = q.x() * q.z();
      f32 qxy = q.x() * q.y();
      f32 qyz = q.y() * q.z();
      f32 qwx = q.w() * q.x();
      f32 qwy = q.w() * q.y();
      f32 qwz = q.w() * q.z();

      data[0][0] = 1.0f - 2.0f * (qyy + qzz);
      data[0][1] =        2.0f * (qxy + qwz);
      data[0][2] =        2.0f * (qxz - qwy);

      data[1][0] =        2.0f * (qxy - qwz);
      data[1][1] = 1.0f - 2.0f * (qxx + qzz);
      data[1][2] =        2.0f * (qyz + qwx);

      data[2][0] =        2.0f * (qxz + qwy);
      data[2][1] =        2.0f * (qyz - qwx);
      data[2][2] = 1.0f - 2.0f * (qxx + qyy);
    }

  Mat4 operator*(const Mat4 &b) const
  {
    Mat4 res;
    for(auto i=0; i < 4; i++) {
      for(auto j=0; j < 4; j++) {
        res.data[j][i] = data[0][i] * b.data[j][0] +
                         data[1][i] * b.data[j][1] +
                         data[2][i] * b.data[j][2] +
                         data[3][i] * b.data[j][3];
      }
    }
    return res;
  }

  Vec3 operator*(const Vec3 &b) const
  {
    Vec3 res;
    for(auto i=0; i < 3; i++) {
      res.data[i] = data[0][i] * b[0] +
                    data[1][i] * b[1] +
                    data[2][i] * b[2] +
                    data[3][i] * 1.0f;
    }
    return res;
  }

  Mat4 operator*(f32 b) const
  {
    Mat4 res;
    for(auto i=0; i < 4; i++) {
      for(auto j=0; j < 4; j++) {
        res.data[i][j] = data[i][j] * b;
      }
    }
    return res;
  }

  Mat4& operator*=(const Mat4& b) { return *this = *this * b; }
  Mat4& operator*=(f32 b) { return *this = *this * b; }
};