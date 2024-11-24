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

void t3d_mat4_to_frustum(T3DFrustum *frustum, const T3DMat4 *mat) {
 for(int i=0; i<4; ++i) {
    frustum->planes[0].v[i] = mat->m[i][3] + mat->m[i][0]; // left
    frustum->planes[1].v[i] = mat->m[i][3] - mat->m[i][0]; // right
    frustum->planes[2].v[i] = mat->m[i][3] + mat->m[i][1]; // bottom
    frustum->planes[3].v[i] = mat->m[i][3] - mat->m[i][1]; // top
    frustum->planes[4].v[i] = mat->m[i][3] + mat->m[i][2]; // near
    frustum->planes[5].v[i] = mat->m[i][3] - mat->m[i][2]; // far
 }
  for(int i=0; i<6; ++i) {
    float len = t3d_vec3_len((T3DVec3*)&frustum->planes[i]);
    frustum->planes[i].v[0] /= len;
    frustum->planes[i].v[1] /= len;
    frustum->planes[i].v[2] /= len;
    frustum->planes[i].v[3] /= len;
  }
}

void t3d_frustum_scale(T3DFrustum *frustum, float scale) {
  for(int i=0; i<6; ++i) {
    frustum->planes[i].v[0] *= scale;
    frustum->planes[i].v[1] *= scale;
    frustum->planes[i].v[2] *= scale;
  }
}

bool t3d_frustum_vs_aabb(const T3DFrustum *frustum, const T3DVec3 *min, const T3DVec3 *max)
{
  for(int i=0; i<6; ++i) {
    float p0Min = frustum->planes[i].v[0] * (min->v[0]);
    float p0Max = frustum->planes[i].v[0] * (max->v[0]);
    float p1Min = frustum->planes[i].v[1] * (min->v[1]);
    float p1Max = frustum->planes[i].v[1] * (max->v[1]);

    float p2MinAndW = -frustum->planes[i].v[3] - frustum->planes[i].v[2] * (min->v[2]);
    if(p0Min + p1Min > p2MinAndW) continue;
    if(p0Max + p1Min > p2MinAndW) continue;
    if(p0Min + p1Max > p2MinAndW) continue;
    if(p0Max + p1Max > p2MinAndW) continue;

    float p2MaxAndW = -frustum->planes[i].v[3] - frustum->planes[i].v[2] * (max->v[2]);
    if(p0Min + p1Min > p2MaxAndW) continue;
    if(p0Max + p1Min > p2MaxAndW) continue;
    if(p0Min + p1Max > p2MaxAndW) continue;
    if(p0Max + p1Max > p2MaxAndW) continue;
    return false;
  }
  return true;
}

bool t3d_frustum_vs_aabb_s16(const T3DFrustum *frustum, const int16_t min[3], const int16_t max[3])
{
  for(int i=0; i<6; ++i) {
    float p0Min = frustum->planes[i].v[0] * (min[0]);
    float p0Max = frustum->planes[i].v[0] * (max[0]);
    float p1Min = frustum->planes[i].v[1] * (min[1]);
    float p1Max = frustum->planes[i].v[1] * (max[1]);

    float p2MinAndW = -frustum->planes[i].v[3] - frustum->planes[i].v[2] * (min[2]);
    if(p0Min + p1Min > p2MinAndW) continue;
    if(p0Max + p1Min > p2MinAndW) continue;
    if(p0Min + p1Max > p2MinAndW) continue;
    if(p0Max + p1Max > p2MinAndW) continue;

    float p2MaxAndW = -frustum->planes[i].v[3] - frustum->planes[i].v[2] * (max[2]);
    if(p0Min + p1Min > p2MaxAndW) continue;
    if(p0Max + p1Min > p2MaxAndW) continue;
    if(p0Min + p1Max > p2MaxAndW) continue;
    if(p0Max + p1Max > p2MaxAndW) continue;
    return false;
  }
  return true;
}

bool t3d_frustum_vs_sphere(const T3DFrustum *frustum, const T3DVec3 *center, const float radius){
  for(int i=0; i<6; ++i) {
    float dist = t3d_vec3_dot((T3DVec3*)&frustum->planes[i], center) + frustum->planes[i].v[3];
    if(dist < -radius) return false;
  }
  return true;
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

void t3d_mat4_to_fixed(T3DMat4FP *matOut, const T3DMat4 *matIn) {
  for(uint32_t y=0; y<4; ++y) {
    // unrolling the inner loop here is faster, even though code size gets bigger (~1.8x)
    uint32_t fixed0 = T3D_F32_TO_FIXED(matIn->m[y][0]);
    uint32_t fixed1 = T3D_F32_TO_FIXED(matIn->m[y][1]);
    uint32_t fixed2 = T3D_F32_TO_FIXED(matIn->m[y][2]);
    uint32_t fixed3 = T3D_F32_TO_FIXED(matIn->m[y][3]);

    // prepare 64bit values, this creates less writes to memory later
    uint64_t I = (fixed0 & 0xFFFF0000) | (fixed1 >> 16);
    I <<= 32; // needs to be separate, otherwise -Os generates wrong code
    I |= (fixed2 & 0xFFFF0000) | (fixed3 >> 16);

    uint64_t F = (fixed0 << 16) | (fixed1 & 0xFFFF);
    F <<= 32; // needs to be separate, otherwise -Os generates wrong code
    F |= (fixed2 << 16) | (fixed3 & 0xFFFF);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
      *(uint64_t*)matOut->m[y].i = I; // guaranteed to be 64-bit aligned
      *(uint64_t*)matOut->m[y].f = F;
    #pragma GCC diagnostic pop
  }
}

void t3d_mat4_to_fixed_3x4(T3DMat4FP *matOut, const T3DMat4 *matIn) {
  for(uint32_t y=0; y<4; ++y) {
    uint32_t fixed0 = T3D_F32_TO_FIXED(matIn->m[y][0]);
    uint32_t fixed1 = T3D_F32_TO_FIXED(matIn->m[y][1]);
    uint32_t fixed2 = T3D_F32_TO_FIXED(matIn->m[y][2]);

    // prepare 64bit values, this creates less writes to memory later
    uint64_t I = (fixed0 & 0xFFFF0000) | (fixed1 >> 16);
    I <<= 32; // needs to be separate, otherwise -Os generates wrong code
    I |= (fixed2 & 0xFFFF0000);

    // puts a '1' into the last value of the matrix (seems to be faster than 'I |= y > 2')
    I |= (y+1) >> 2;

    uint64_t F = (fixed0 << 16) | (fixed1 & 0xFFFF);
    F <<= 32; // needs to be separate, otherwise -Os generates wrong code
    F |= (fixed2 << 16);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
      *(uint64_t*)matOut->m[y].i = I; // guaranteed to be 64-bit aligned
      *(uint64_t*)matOut->m[y].f = F;
    #pragma GCC diagnostic pop
  }
}

void t3d_mat4_ortho(T3DMat4 *mat, float left, float right, float bottom, float top, float near, float far) {
  *mat = (T3DMat4){0};
  mat->m[0][0] = 2.0f / (right - left);
  mat->m[1][1] = 2.0f / (top - bottom);
  mat->m[2][2] = -2.0f / (far - near);
  mat->m[3][0] = -(right + left) / (right - left);
  mat->m[3][1] = -(top + bottom) / (top - bottom);
  mat->m[3][2] = -(far + near) / (far - near);
  mat->m[3][3] = 1.0f;
}

void t3d_mat4_from_srt(T3DMat4 *mat, const float scale[3], const  float quat[4], const  float translate[3])
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

void t3d_mat4_from_srt_euler(T3DMat4 *mat, const float scale[3], const float rot[3], const float translate[3])
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

void t3d_mat4_rot_from_dir(T3DMat4 *mat, const T3DVec3 *dir, const T3DVec3 *up) {
  T3DVec3 tangent = (T3DVec3){{1.0f, 0.0f, 0.0f}};
  T3DVec3 binormal = (T3DVec3){{0.0f, 0.0f, 1.0f}};

  // (near)-up normals cause invalid results
  if(dir->v[1] < 0.999f) {
    t3d_vec3_cross(&tangent, up, dir);
    t3d_vec3_norm(&tangent);
    t3d_vec3_cross(&binormal, dir, &tangent);
  }

  *mat = (T3DMat4){{
    {tangent.v[0],  tangent.v[1],  tangent.v[2],  0.0f},
    {dir->v[0],     dir->v[1],     dir->v[2],     0.0f},
    {binormal.v[0], binormal.v[1], binormal.v[2], 0.0f},
    {0.0f,          0.0f,          0.0f,          1.0f},
  }};
}

void t3d_mat4fp_from_srt_euler(T3DMat4FP *mat, const float scale[3], const float rot[3], const float translate[3]) {
  T3DMat4 matF;
  t3d_mat4_from_srt_euler(&matF, scale, rot, translate);
  t3d_mat4_to_fixed_3x4(mat, &matF);
}

void t3d_mat4fp_from_srt(T3DMat4FP *mat, const float scale[3], const float rotQuat[4], const float translate[3]) {
  T3DMat4 matF;
  t3d_mat4_from_srt(&matF, scale, rotQuat, translate);
  t3d_mat4_to_fixed_3x4(mat, &matF);
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

