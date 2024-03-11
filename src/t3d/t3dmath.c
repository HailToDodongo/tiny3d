/**
* @copyright 2023 - Max Bebök
* @license MIT
*/

#include <t3d/t3dmath.h>

void t3d_mat4_look_at(T3DMat4 *mat, const T3DVec3 *eye, const T3DVec3 *target) {
  T3DVec3 dist;
  t3d_vec3_diff(&dist, target, eye);
  t3d_vec3_norm(&dist);

  T3DVec3 normAx;
  t3d_vec3_cross_up(&normAx, &dist);
  t3d_vec3_norm(&normAx);

  T3DVec3 normRelAxis;
  t3d_vec3_cross(&normRelAxis, &normAx, &dist);

  float dotNorm = -t3d_vec3_dot(&normAx, eye);
  float dotRel  = -t3d_vec3_dot(&normRelAxis, eye);
  float dotDist = t3d_vec3_dot(&dist, eye);

  *mat = (T3DMat4){{
    {normAx.v[0], normRelAxis.v[0], -dist.v[0], 0.0f},
    {normAx.v[1], normRelAxis.v[1], -dist.v[1], 0.0f},
    {normAx.v[2], normRelAxis.v[2], -dist.v[2], 0.0f},
    {dotNorm,     dotRel,            dotDist,   1.0f},
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

void t3d_mat4_from_srt(T3DMat4 *mat, float scale[3], float rot[4], float translate[3])
{
  assertf(false, "TODO: implement t3d_mat4_from_srt()");
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

void t3d_mat4_rotate(T3DMat4 *mat, const T3DVec3* axis, float angleRad)
{
  float c, s;
  fm_sincosf(angleRad, &s, &c);
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
