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
  float x = rot[0];
  float y = rot[1];
  float z = rot[2];
  float w = rot[3];

  float x2 = x + x;
  float y2 = y + y;
  float z2 = z + z;
  float xx = x * x2;
  float xy = x * y2;
  float xz = x * z2;
  float yy = y * y2;
  float yz = y * z2;
  float zz = z * z2;
  float wx = w * x2;
  float wy = w * y2;
  float wz = w * z2;

  float sx = scale[0];
  float sy = scale[1];
  float sz = scale[2];

  mat->m[0][0] = (1.0f - (yy + zz)) * sx;
  mat->m[0][1] = (xy + wz) * sx;
  mat->m[0][2] = (xz - wy) * sx;
  mat->m[0][3] = 0.0f;

  mat->m[1][0] = (xy - wz) * sy;
  mat->m[1][1] = (1.0f - (xx + zz)) * sy;
  mat->m[1][2] = (yz + wx) * sy;
  mat->m[1][3] = 0.0f;

  mat->m[2][0] = (xz + wy) * sz;
  mat->m[2][1] = (yz - wx) * sz;
  mat->m[2][2] = (1.0f - (xx + yy)) * sz;
  mat->m[2][3] = 0.0f;

  mat->m[3][0] = translate[0];
  mat->m[3][1] = translate[1];
  mat->m[3][2] = translate[2];
  mat->m[3][3] = 1.0f;
}

void t3d_mat4_from_srt_euler(T3DMat4 *mat, float scale[3], float rot[3], float translate[3])
{
  float sx = scale[0];
  float sy = scale[1];
  float sz = scale[2];

  float cx = cosf(rot[0]);
  float cy = cosf(rot[1]);
  float cz = cosf(rot[2]);

  mat->m[0][0] = cy * cz * sx;
  mat->m[0][1] = cy * cz * cx;
  mat->m[0][2] = -cy * sz;
  mat->m[0][3] = 0.0f;

  mat->m[1][0] = (sx * sy * cz) - (sz * cx);
  mat->m[1][1] = (sx * sy * sz) + (cz * cx);
  mat->m[1][2] = sx * cy;
  mat->m[1][3] = 0.0f;

  mat->m[2][0] = (cx * sy * cz) + (sz * sx);
  mat->m[2][1] = (cx * sy * sz) - (cz * sx);
  mat->m[2][2] = cx * cy;
  mat->m[2][3] = 0.0f;

  mat->m[3][0] = translate[0];
  mat->m[3][1] = translate[1];
  mat->m[3][2] = translate[2];
  mat->m[3][3] = 1.0f;
}

void t3d_mat4_rotate(T3DMat4 *mat, const T3DVec3* axis, float angleRad)
{
  float c = fm_cosf(angleRad);
  float s = fm_sinf(angleRad);
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