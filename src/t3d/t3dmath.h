/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#ifndef TINY3D_T3DMATH_H
#define TINY3D_T3DMATH_H

#include <libdragon.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define T3D_DEG_TO_RAD(deg) (deg * 0.01745329252f)
#define T3D_F32_TO_FIXED(val) (int32_t)((val) * (float)(1<<16))

// 3D float vector
typedef struct {
  float v[3];
} T3DVec3;

// float quaternion
typedef struct {
  float v[4];
} T3DQuat;

// 4D float vector
typedef struct {
  float m[4][4];
} T3DMat4;

// 3D s16.16 fixed-point vector, used as-is by the RSP.
typedef struct {
  int16_t i[4];
  uint16_t f[4];
} T3DVec4FP;

// 4x4 Matrix of 16.16 fixed-point numbers, used as-is by the RSP.
typedef struct {
  T3DVec4FP m[4];
} __attribute__((aligned(16))) T3DMat4FP;

/// @brief Converts a 16.16 fixed-point number to a float
inline static float s1616_to_float(int16_t partI, uint16_t partF)
{
  float res = (float)partI;
  res += (float)partF / 65536.f;
  return res;
}

/// @brief Sets 'res' to 'a + b'
inline static void t3d_vec3_add(T3DVec3 *res, const T3DVec3 *a, const T3DVec3 *b) {
  res->v[0] = a->v[0] + b->v[0];
  res->v[1] = a->v[1] + b->v[1];
  res->v[2] = a->v[2] + b->v[2];
}

/// @brief Sets 'res' to 'a + b'
inline static void t3d_vec3_mul(T3DVec3 *res, const T3DVec3 *a, const T3DVec3 *b) {
  res->v[0] = a->v[0] * b->v[0];
  res->v[1] = a->v[1] * b->v[1];
  res->v[2] = a->v[2] * b->v[2];
}

/// @brief Set 'res' to 'a - b'
inline static void t3d_vec3_diff(T3DVec3 *res, const T3DVec3 *a, const T3DVec3 *b) {
  res->v[0] = a->v[0] - b->v[0];
  res->v[1] = a->v[1] - b->v[1];
  res->v[2] = a->v[2] - b->v[2];
}

/// @brief Returns the squared length of 'v'
inline static float t3d_vec3_len2(T3DVec3 *vec) {
  return vec->v[0] * vec->v[0]
       + vec->v[1] * vec->v[1]
       + vec->v[2] * vec->v[2];
}

/// @brief Normalizes 'res', this does NOT check for a zero-length vector
inline static void t3d_vec3_norm(T3DVec3 *res) {
  float len = sqrtf(t3d_vec3_len2(res));
  if(len < 0.0001f)len = 0.0001f;
  res->v[0] /= len;
  res->v[1] /= len;
  res->v[2] /= len;
}

/// @brief Crosses 'a' with 'b' and stores it in 'res'
inline static void t3d_vec3_cross(T3DVec3 *res, const T3DVec3 *a, const T3DVec3 *b) {
  res->v[0] = a->v[1]*b->v[2] - b->v[1]*a->v[2];
  res->v[1] = a->v[2]*b->v[0] - b->v[2]*a->v[0];
  res->v[2] = a->v[0]*b->v[1] - b->v[0]*a->v[1];
}

/// @brief Crosses 'a' with the up vector (0, 1, 0) and stores it in 'res'
inline static void t3d_vec3_cross_up(T3DVec3 *res, const T3DVec3 *a) {
  res->v[0] = a->v[2];
  res->v[1] = 0;
  res->v[2] = -a->v[0];
}

/// @brief Returns the dot product of 'a' and 'b'
inline static float t3d_vec3_dot(const T3DVec3 *a, const T3DVec3 *b) {
  return a->v[0] * b->v[0] +
         a->v[1] * b->v[1] +
         a->v[2] * b->v[2];
}

/**
 * @brief Initializes a matrix to the identity matrix
 * @param mat matrix to be changed
 */
inline static void t3d_mat4_identity(T3DMat4 *mat)
{
  *mat = (T3DMat4){{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
  }};
}

/**
 * Scales a matrix by the given factors
 * @param mat result
 * @param scaleX
 * @param scaleY
 * @param scaleZ
 */
inline static void t3d_mat4_scale(T3DMat4 *mat, float scaleX, float scaleY, float scaleZ)
{
  mat->m[0][0] *= scaleX;
  mat->m[0][1] *= scaleX;
  mat->m[0][2] *= scaleX;
  mat->m[0][3] *= scaleX;

  mat->m[1][0] *= scaleY;
  mat->m[1][1] *= scaleY;
  mat->m[1][2] *= scaleY;
  mat->m[1][3] *= scaleY;

  mat->m[2][0] *= scaleZ;
  mat->m[2][1] *= scaleZ;
  mat->m[2][2] *= scaleZ;
  mat->m[2][3] *= scaleZ;
}

/**
 * Translates a matrix by the given offsets
 * @param mat result
 * @param offsetX
 * @param offsetY
 * @param offsetZ
 */
inline static void t3d_mat4_translate(T3DMat4 *mat, float offsetX, float offsetY, float offsetZ)
{
  mat->m[3][0] += offsetX;
  mat->m[3][1] += offsetY;
  mat->m[3][2] += offsetZ;
}

/**
 * Rotates a matrix around an axis
 * @param mat result
 * @param axis axis to rotate around
 * @param angleRad angle in radians
 */
void t3d_mat4_rotate(T3DMat4 *mat, const T3DVec3* axis, float angleRad);

/**
 * Directly constructs a matrix from scale, rotation (quaternion) and translation
 * For a euler version, see 't3d_mat4_from_srt_euler'.
 *
 * @param mat result
 * @param scale scale factors
 * @param rot rotation quaternion
 * @param translate offsets
 */
void t3d_mat4_from_srt(T3DMat4 *mat, float scale[3], float rot[4], float translate[3]);

/**
 * Directly constructs a matrix from scale, rotation (euler) and translation
 * For a quaternion version, see 't3d_mat4_from_srt'.
 *
 * @param mat
 * @param scale
 * @param rot
 * @param translate
 */
void t3d_mat4_from_srt_euler(T3DMat4 *mat, float scale[3], float rot[3], float translate[3]);

/**
 * Directly constructs a matrix from scale, rotation (euler) and translation
 * Same as 't3d_mat4_from_srt_euler', but instead directly writes to a fixed-point matrix.
 * @param mat
 * @param scale
 * @param rot
 * @param translate
 */
void t3d_mat4fp_from_srt_euler(T3DMat4FP *mat, float scale[3], float rot[3], float translate[3]);

/**
 * @brief Sets a value in a fixed-point matrix
 * @param mat matrix to be changed
 * @param y row
 * @param x column
 * @param val value to be set as a float
 */
inline static void t3d_mat4fp_set_float(T3DMat4FP *mat, uint32_t y, uint32_t x, float val)
{
  int32_t fixed = T3D_F32_TO_FIXED(val);
  mat->m[y].i[x] = (int16_t)(fixed >> 16);
  mat->m[y].f[x] = fixed & 0xFFFF;
}

/**
 * @brief Gets a value from a fixed-point matrix
 * @param mat matrix to be read
 * @param y row
 * @param x column
 * @return value as a float
 */
inline static float t3d_mat4fp_get_float(const T3DMat4FP *mat, uint32_t y, uint32_t x)
{
  return s1616_to_float(mat->m[y].i[x], mat->m[y].f[x]);
}

inline static void t3d_mat4fp_identity(T3DMat4FP *mat)
{
  *mat = (T3DMat4FP){{
    {{1, 0, 0, 0}},
    {{0, 1, 0, 0}},
    {{0, 0, 1, 0}},
    {{0, 0, 0, 1}},
  }};
}
/**
 * Converts a float matrix to a fixed-point matrix.
 * @param matOut result
 * @param matIn input
 */
inline static void t3d_mat4_to_fixed(T3DMat4FP *matOut, const T3DMat4 *matIn)
{
  for(uint32_t y=0; y<4; ++y) {
    for(uint32_t x=0; x<4; ++x) {
      int32_t fixed = T3D_F32_TO_FIXED(matIn->m[y][x]);
      matOut->m[y].i[x] = (int16_t)(fixed >> 16);
      matOut->m[y].f[x] = fixed & 0xFFFF;
    }
  }
}

/**
 * Constructs a perspective projection matrix
 * @param mat result
 * @param fov fov in radians
 * @param aspect aspect ratio
 * @param near near plane distance
 * @param far far plane distance
 */
void t3d_mat4_perspective(T3DMat4 *mat, float fov, float aspect, float near, float far);

/**
 * @brief Creates a look-at matrix from an eye and target vector
 * @param mat destination matrix
 * @param eye camera position
 * @param target camera target/focus point
 * @param up camera up vector, expected to be {0,1,0} by default
 */
void t3d_mat4_look_at(T3DMat4 *mat, const T3DVec3 *eye, const T3DVec3 *target, const T3DVec3 *up);

/// @brief Multiplies the matrices 'matA' and 'matB' and stores it in 'matRes'
inline static void t3d_mat4_mul(T3DMat4 *matRes, const T3DMat4 *matA, const T3DMat4 *matB)
{
  for(uint32_t i=0; i<4; i++) {
    for(uint32_t  j=0; j<4; j++) {
      matRes->m[j][i] = matA->m[0][i] * matB->m[j][0] +
                        matA->m[1][i] * matB->m[j][1] +
                        matA->m[2][i] * matB->m[j][2] +
                        matA->m[3][i] * matB->m[j][3];
    }
  }
}

/**
 * Multiplies a 3x3 matrix with a 3D vector
 * @param vecOut result
 * @param mat matrix
 * @param vec input-vector
 */
inline static void t3d_mat3_mul_vec3(T3DVec3* vecOut, const T3DMat4 *mat, const T3DVec3* vec)
{
  for(uint32_t i=0; i<3; i++) {
    vecOut->v[i] = mat->m[0][i] * vec->v[0] +
                   mat->m[1][i] * vec->v[1] +
                   mat->m[2][i] * vec->v[2];
  }
}

#ifdef __cplusplus
}
#endif

#endif //TINY3D_T3DMATH_H
