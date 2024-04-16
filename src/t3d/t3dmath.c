/**
* @copyright 2023 - Max Bebök
* @license MIT
*/

#include <t3d/t3dmath.h>

void t3d_quat_nlerp(T3DQuat *res, const T3DQuat *a, const T3DQuat *b, float t) {
  float blend = 1.0f - t;
  if(t3d_quat_dot(a, b) < 0.0f) {
    blend = -blend;
  }
  res->v[0] = blend * a->v[0] + t * b->v[0];
  res->v[1] = blend * a->v[1] + t * b->v[1];
  res->v[2] = blend * a->v[2] + t * b->v[2];
  res->v[3] = blend * a->v[3] + t * b->v[3];
  t3d_quat_normalize(res);
}

void t3d_quat_slerp(T3DQuat *res, const T3DQuat *a, const T3DQuat *b, float t) {
  float dot = t3d_quat_dot(a, b);
  float negated = 1.0f;
  if(dot < 0.0f) {
    dot = -dot;
    negated = -1;
  }

  if(dot < 0.99999f)
  {
    float angle = acosf(dot);
    float sinAngle = fm_sinf(angle);
    float invSinAngle = 1.0f / sinAngle;
    float coeff0 = fm_sinf((1.0f - t) * angle) * invSinAngle;
    float coeff1 = fm_sinf(t * angle) * invSinAngle * negated;
    res->v[0] = coeff0 * a->v[0] + coeff1 * b->v[0];
    res->v[1] = coeff0 * a->v[1] + coeff1 * b->v[1];
    res->v[2] = coeff0 * a->v[2] + coeff1 * b->v[2];
    res->v[3] = coeff0 * a->v[3] + coeff1 * b->v[3];
  }
}

void t3d_mat4_look_at(T3DMat4 *mat, const T3DVec3 *eye, const T3DVec3 *target, const T3DVec3 *up)
{
  T3DVec3 forward, side, upCalc;

  t3d_vec3_diff(&forward, target, eye);
  t3d_vec3_norm(&forward);

  t3d_vec3_cross(&side, &forward, up);
  t3d_vec3_norm(&side);

  t3d_vec3_cross(&upCalc, &side, &forward);

  float dotSide = -t3d_vec3_dot(&side, eye);
  float dotUp   = -t3d_vec3_dot(&upCalc, eye);
  float dotFwd  = t3d_vec3_dot(&forward, eye);

  *mat = (T3DMat4){{
    {side.v[0], upCalc.v[0], -forward.v[0], 0.0f},
    {side.v[1], upCalc.v[1], -forward.v[1], 0.0f},
    {side.v[2], upCalc.v[2], -forward.v[2], 0.0f},
    {dotSide,   dotUp,        dotFwd,       1.0f}
  }};
}

void t3d_mat4_perspective(T3DMat4 *mat, float fov, float aspect, float near, float far) {
  float tanHalfFov = tanf(fov * 0.5f);
  *mat = (T3DMat4){0};
  mat->m[0][0] = 1.0f / (aspect * tanHalfFov);
  mat->m[1][1] = 1.0f / tanHalfFov;
  mat->m[2][2] = far / (near - far);
  mat->m[2][3] = -1.0f;
  mat->m[3][2] = -2.0f * (far * near) / (far - near);
}

void t3d_mat4_from_srt(T3DMat4 *mat, float scale[3], float quat[4], float translate[3])
{
  float qxx = quat[0] * quat[0];
  float qyy = quat[1] * quat[1];
  float qzz = quat[2] * quat[2];
  float qxz = quat[0] * quat[2];
  float qxy = quat[0] * quat[1];
  float qyz = quat[1] * quat[2];
  float qwx = quat[3] * quat[0];
  float qwy = quat[3] * quat[1];
  float qwz = quat[3] * quat[2];
  
  *mat = (T3DMat4){{
    {1.0f - 2.0f * (qyy + qzz),        2.0f * (qxy + qwz),        2.0f * (qxz - qwy), 0.0f},
    {       2.0f * (qxy - qwz), 1.0f - 2.0f * (qxx + qzz),        2.0f * (qyz + qwx), 0.0f},
    {       2.0f * (qxz + qwy),        2.0f * (qyz - qwx), 1.0f - 2.0f * (qxx + qyy), 0.0f},
    {             translate[0],              translate[1],              translate[2], 1.0f}
  }};
  t3d_mat4_scale(mat, scale[0], scale[1], scale[2]);
}

void t3d_mat4_from_srt_euler(T3DMat4 *mat, float scale[3], float rot[3], float translate[3])
{
  float cosR0 = fm_cosf(rot[0]);
  float cosR2 = fm_cosf(rot[2]);
  float cosR1 = fm_cosf(rot[1]);

  float sinR0 = fm_sinf(rot[0]);
  float sinR1 = fm_sinf(rot[1]);
  float sinR2 = fm_sinf(rot[2]);

  *mat = (T3DMat4){{
    {scale[0] * cosR2 * cosR1, scale[0] * (cosR2 * sinR1 * sinR0 - sinR2 * cosR0), scale[0] * (cosR2 * sinR1 * cosR0 + sinR2 * sinR0), 0.0f},
    {scale[1] * sinR2 * cosR1, scale[1] * (sinR2 * sinR1 * sinR0 + cosR2 * cosR0), scale[1] * (sinR2 * sinR1 * cosR0 - cosR2 * sinR0), 0.0f},
    {-scale[2] * sinR1, scale[2] * cosR1 * sinR0, scale[2] * cosR1 * cosR0, 0.0f},
    {translate[0], translate[1], translate[2], 1.0f}
  }};
}

void t3d_mat4fp_from_srt_euler(T3DMat4FP *mat, float scale[3], float rot[3], float translate[3]) {
  T3DMat4 matF; // @TODO: avoid temp matrix
  t3d_mat4_from_srt_euler(&matF, scale, rot, translate);
  t3d_mat4_to_fixed(mat, &matF);
}

void t3d_mat4fp_from_srt(T3DMat4FP *mat, float scale[3], float rotQuat[4], float translate[3]) {
  T3DMat4 matF; // @TODO: avoid temp matrix
  t3d_mat4_from_srt(&matF, scale, rotQuat, translate);
  t3d_mat4_to_fixed(mat, &matF);
}

void t3d_mat4_rotate(T3DMat4 *mat, const T3DVec3* axis, float angleRad)
{
  float s, c;
  // @TODO: currently buggy in libdragon, use once fixed
  // fm_sincosf(angleRad, &s, &c);
  s = fm_sinf(angleRad);
  c = fm_cosf(angleRad);

  float t = 1.0f - c;

  float x = axis->v[0];
  float y = axis->v[1];
  float z = axis->v[2];

  mat->m[0][0] = t * x * x + c;
  mat->m[0][1] = t * x * y - s * z;
  mat->m[0][2] = t * x * z + s * y;
  mat->m[0][3] = 0.0f;

  mat->m[1][0] = t * x * y + s * z;
  mat->m[1][1] = t * y * y + c;
  mat->m[1][2] = t * y * z - s * x;
  mat->m[1][3] = 0.0f;

  mat->m[2][0] = t * x * z - s * y;
  mat->m[2][1] = t * y * z + s * x;
  mat->m[2][2] = t * z * z + c;
  mat->m[2][3] = 0.0f;

  mat->m[3][0] = 0.0f;
  mat->m[3][1] = 0.0f;
  mat->m[3][2] = 0.0f;
  mat->m[3][3] = 1.0f;
}

