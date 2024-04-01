/**
* @copyright 2022 - Max Bebök
* @license MIT
*/
#pragma once

#include <cstdio>
#include <string>
#include "../types.h"
#include "vec3.h"
#include "quat.h"
#include "vec4.h"

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

  explicit Mat4(const Vec4 &c0, const Vec4 &c1, const Vec4 &c2, const Vec4 &c3) : data{
    {c0[0], c0[1], c0[2], c0[3]},
    {c1[0], c1[1], c1[2], c1[3]},
    {c2[0], c2[1], c2[2], c2[3]},
    {c3[0], c3[1], c3[2], c3[3]},
  } {}

  Vec4& operator[](u64 column) { return *(Vec4*)data[column]; }
  const Vec4& operator[](u64 column) const { return *(Vec4*)data[column]; }

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

  void scale(const  Vec3& scale) {
    data[0][0] *= scale[0];
    data[0][1] *= scale[0];
    data[0][2] *= scale[0];

    data[1][0] *= scale[1];
    data[1][1] *= scale[1];
    data[1][2] *= scale[1];

    data[2][0] *= scale[2];
    data[2][1] *= scale[2];
    data[2][2] *= scale[2];
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

  Mat4 inverse() const {
      f32 coef00 = data[2][2] * data[3][3] - data[3][2] * data[2][3];
      f32 coef02 = data[1][2] * data[3][3] - data[3][2] * data[1][3];
      f32 coef03 = data[1][2] * data[2][3] - data[2][2] * data[1][3];

      f32 coef04 = data[2][1] * data[3][3] - data[3][1] * data[2][3];
      f32 coef06 = data[1][1] * data[3][3] - data[3][1] * data[1][3];
      f32 coef07 = data[1][1] * data[2][3] - data[2][1] * data[1][3];

      f32 coef08 = data[2][1] * data[3][2] - data[3][1] * data[2][2];
      f32 coef10 = data[1][1] * data[3][2] - data[3][1] * data[1][2];
      f32 coef11 = data[1][1] * data[2][2] - data[2][1] * data[1][2];

      f32 coef12 = data[2][0] * data[3][3] - data[3][0] * data[2][3];
      f32 coef14 = data[1][0] * data[3][3] - data[3][0] * data[1][3];
      f32 coef15 = data[1][0] * data[2][3] - data[2][0] * data[1][3];

      f32 coef16 = data[2][0] * data[3][2] - data[3][0] * data[2][2];
      f32 coef18 = data[1][0] * data[3][2] - data[3][0] * data[1][2];
      f32 coef19 = data[1][0] * data[2][2] - data[2][0] * data[1][2];

      f32 coef20 = data[2][0] * data[3][1] - data[3][0] * data[2][1];
      f32 coef22 = data[1][0] * data[3][1] - data[3][0] * data[1][1];
      f32 coef23 = data[1][0] * data[2][1] - data[2][0] * data[1][1];

      Vec4 factor0{coef00, coef00, coef02, coef03};
      Vec4 factor1{coef04, coef04, coef06, coef07};
      Vec4 factor2{coef08, coef08, coef10, coef11};
      Vec4 factor3{coef12, coef12, coef14, coef15};
      Vec4 factor4{coef16, coef16, coef18, coef19};
      Vec4 factor5{coef20, coef20, coef22, coef23};

      Vec4 Vec0{data[1][0],data[0][0],data[0][0],data[0][0]};
      Vec4 Vec1{data[1][1],data[0][1],data[0][1],data[0][1]};
      Vec4 Vec2{data[1][2],data[0][2],data[0][2],data[0][2]};
      Vec4 Vec3{data[1][3],data[0][3],data[0][3],data[0][3]};

      Vec4 signA{ 1.0f, -1.0f,  1.0f, -1.0f};
      Vec4 signB{-1.0f,  1.0f, -1.0f,  1.0f};

      auto Inv0 = Vec4{Vec1 * factor0 - Vec2 * factor1 + Vec3 * factor2} * signA;
      auto Inv1 = Vec4{Vec0 * factor0 - Vec2 * factor3 + Vec3 * factor4} * signB;
      auto Inv2 = Vec4{Vec0 * factor1 - Vec1 * factor3 + Vec3 * factor5} * signA;
      auto Inv3 = Vec4{Vec0 * factor2 - Vec1 * factor4 + Vec2 * factor5} * signB;

      Mat4 invMat{Inv0, Inv1, Inv2, Inv3};
      Vec4 firstRow{invMat[0][0], invMat[1][0], invMat[2][0], invMat[3][0]};
      f32 dot = (*this)[0].dot(firstRow);
      return invMat * (1.0f / dot);
    }

  Mat4& operator*=(const Mat4& b) { return *this = *this * b; }
  Mat4& operator*=(f32 b) { return *this = *this * b; }

  void print(int indent) const {
    std::string indentStr(indent, ' ');
    printf("%s%f %f %f %f\n", indentStr.c_str(), data[0][0], data[1][0], data[2][0], data[3][0]);
    printf("%s%f %f %f %f\n", indentStr.c_str(), data[0][1], data[1][1], data[2][1], data[3][1]);
    printf("%s%f %f %f %f\n", indentStr.c_str(), data[0][2], data[1][2], data[2][2], data[3][2]);
    printf("%s%f %f %f %f\n", indentStr.c_str(), data[0][3], data[1][3], data[2][3], data[3][3]);
  }
};