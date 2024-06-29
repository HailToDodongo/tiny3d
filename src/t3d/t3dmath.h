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
#define T3D_PI 3.14159265358979f

// 3D float vector
typedef struct {
  float v[3];
} T3DVec3;

typedef struct {
  float v[4];
} T3DVec4;

// float quaternion, stored as XYZW
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

/// @brief Interpolates between two floats by 't'
inline static float t3d_lerp(float a, float b, float t) {
  return a + (b - a) * t;
}

/// @brief Interpolates between two angles (radians) by 't'
inline static float t3d_lerp_angle(float a, float b, float t) {
  float angleDiff = fmodf((b - a), T3D_PI*2);
  float shortDist = fmodf(angleDiff*2, T3D_PI*2) - angleDiff;
  return a + shortDist * t;
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
inline static float t3d_vec3_len2(const T3DVec3 *vec) {
  return vec->v[0] * vec->v[0]
       + vec->v[1] * vec->v[1]
       + vec->v[2] * vec->v[2];
}

/// @brief Returns the length of 'v'
inline static float t3d_vec3_len(const T3DVec3 *vec) {
  return sqrtf(t3d_vec3_len2(vec));
}

/// @brief Returns the squared distance between 'vecA' and 'vecB'
inline static float t3d_vec3_distance2(const T3DVec3 *vecA, const T3DVec3 *vecB) {
  T3DVec3 diff;
  t3d_vec3_diff(&diff, vecA, vecB);
  return t3d_vec3_len2(&diff);
}

/// @brief Returns the distance between 'vecA' and 'vecB'
inline static float t3d_vec3_distance(const T3DVec3 *vecA, const T3DVec3 *vecB) {
  return sqrtf(t3d_vec3_distance2(vecA, vecB));
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

/// @brief Returns the dot product of 'a' and 'b'
inline static float t3d_vec3_dot(const T3DVec3 *a, const T3DVec3 *b) {
  return a->v[0] * b->v[0] +
         a->v[1] * b->v[1] +
         a->v[2] * b->v[2];
}

/// @brief Linearly interpolates between 'a' and 'b' by 't' and stores it in 'res'
inline static void t3d_vec3_lerp(T3DVec3 *res, const T3DVec3 *a, const T3DVec3 *b, float t)
{
  res->v[0] = a->v[0] + (b->v[0] - a->v[0]) * t;
  res->v[1] = a->v[1] + (b->v[1] - a->v[1]) * t;
  res->v[2] = a->v[2] + (b->v[2] - a->v[2]) * t;
}

// @brief Resets a quaternion to the identity quaternion
inline static void t3d_quat_identity(T3DQuat *quat) {
  *quat = (T3DQuat){{0, 0, 0, 1}};
}

/**
 * Creates a quaternion from euler angles (radians)
 * @param quat result (in XYZW order)
 * @param x
 * @param y
 * @param z
 */
inline static void t3d_quat_from_euler(T3DQuat *quat, const float rotEuler[3])
{
  float c1 = fm_cosf(rotEuler[0] / 2.0f);
  float s1 = fm_sinf(rotEuler[0] / 2.0f);
  float c2 = fm_cosf(rotEuler[1] / 2.0f);
  float s2 = fm_sinf(rotEuler[1] / 2.0f);
  float c3 = fm_cosf(rotEuler[2] / 2.0f);
  float s3 = fm_sinf(rotEuler[2] / 2.0f);

  quat->v[0] = c1 * c2 * s3 - s1 * s2 * c3;
  quat->v[1] = s1 * c2 * c3 - c1 * s2 * s3;
  quat->v[2] = c1 * s2 * c3 + s1 * c2 * s3;
  quat->v[3] = c1 * c2 * c3 + s1 * s2 * s3;
}

/**
 * Creates a quaternion from a rotation (radians) around an axis
 * @param quat result
 * @param axis rotation axis
 * @param angleRad angle in radians
 */
inline static void t3d_quat_from_rotation(T3DQuat *quat, float axis[3], float angleRad)
{
  float s = fm_sinf(angleRad / 2.0f);
  float c = fm_cosf(angleRad / 2.0f);
  *quat = (T3DQuat){{axis[0] * s, axis[1] * s, axis[2] * s, c}};
}

/**
 * Multiplies two quaternions and stores the result in 'res'.
 * NOTE: if 'a' or 'b' point to the same quat. as 'res', the result will be incorrect.
 *
 * @param res result
 * @param a first quaternion
 * @param b second quaternion
 */
inline static void t3d_quat_mul(T3DQuat *res, T3DQuat *a, T3DQuat *b)
{
  res->v[0] = a->v[3] * b->v[0] + a->v[0] * b->v[3] + a->v[1] * b->v[2] - a->v[2] * b->v[1];
  res->v[1] = a->v[3] * b->v[1] - a->v[0] * b->v[2] + a->v[1] * b->v[3] + a->v[2] * b->v[0];
  res->v[2] = a->v[3] * b->v[2] + a->v[0] * b->v[1] - a->v[1] * b->v[0] + a->v[2] * b->v[3];
  res->v[3] = a->v[3] * b->v[3] - a->v[0] * b->v[0] - a->v[1] * b->v[1] - a->v[2] * b->v[2];
}

/**
 * Rotates a quaternion around an axis
 * @param quat quat to modify
 * @param axis rotation axis
 * @param angleRad angle in radians
 */
inline static void t3d_quat_rotate_euler(T3DQuat *quat, float axis[3], float angleRad)
{
  T3DQuat tmp, quatRot;
  t3d_quat_from_rotation(&quatRot, axis, angleRad);
  t3d_quat_mul(&tmp, quat, &quatRot);
  *quat = tmp;
}

/**
 * Dot product of a quaternion as a vec4
 * @param a quaternion
 * @param b quaternion
 * @return dot product
 */
inline static float t3d_quat_dot(const T3DQuat *a, const T3DQuat *b)
{
  return a->v[0] * b->v[0] + a->v[1] * b->v[1] + a->v[2] * b->v[2] + a->v[3] * b->v[3];
}

/**
 * Normalizes a quaternion
 * @param quat
 */
inline static void t3d_quat_normalize(T3DQuat *quat)
{
  float scale = 1.0f / sqrtf(quat->v[0]*quat->v[0] + quat->v[1]*quat->v[1] + quat->v[2]*quat->v[2] + quat->v[3]*quat->v[3]);
  quat->v[0] *= scale;
  quat->v[1] *= scale;
  quat->v[2] *= scale;
  quat->v[3] *= scale;
}

/**
 * Interpolates between two quaternions using a normalized linear interpolation.
 * @param res interpolated quaternion
 * @param a first quaternion
 * @param b second quaternion
 * @param t interpolation factor
 */
void t3d_quat_nlerp(T3DQuat *res, const T3DQuat *a, const T3DQuat *b, float t);

void t3d_quat_slerp(T3DQuat *res, const T3DQuat *a, const T3DQuat *b, float t);

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
void t3d_mat4_from_srt(T3DMat4 *mat, const float scale[3], const float quat[4], const  float translate[3]);

/**
 * Directly constructs a matrix from scale, rotation (euler) and translation
 * For a quaternion version, see 't3d_mat4_from_srt'.
 *
 * @param mat
 * @param scale
 * @param rot
 * @param translate
 */
void t3d_mat4_from_srt_euler(T3DMat4 *mat, const float scale[3], const float rot[3], const float translate[3]);

/**
 * Constructs a matrix from a direction and up vector.
 * This will only create a rotation matrix, the translation part will be identity.
 * @param mat result
 * @param dir direction vector
 * @param up up vector
 */
void t3d_mat4_rot_from_dir(T3DMat4 *mat, const T3DVec3 *dir, const T3DVec3 *up);

/**
 * Directly constructs a matrix from scale, rotation (euler) and translation
 * Same as 't3d_mat4_from_srt_euler', but instead directly writes to a fixed-point matrix.
 * @param mat
 * @param scale
 * @param rot
 * @param translate
 */
void t3d_mat4fp_from_srt_euler(T3DMat4FP *mat, const float scale[3], const float rot[3], const float translate[3]);

/**
 * Directly constructs a matrix from scale, rotation (quaternion) and translation
 * Same as 't3d_mat4_from_srt', but instead directly writes to a fixed-point matrix.
 * @param mat
 * @param scale
 * @param rot
 * @param translate
 */
void t3d_mat4fp_from_srt(T3DMat4FP *mat, const float scale[3], const float rotQuat[4], const float translate[3]);

/**
 * @brief Sets a value in a fixed-point matrix
 * @param mat matrix to be changed
 * @param column
 * @param row
 * @param val value to be set as a float
 */
inline static void t3d_mat4fp_set_float(T3DMat4FP *mat, uint32_t column, uint32_t row, float val)
{
  int32_t fixed = T3D_F32_TO_FIXED(val);
  mat->m[column].i[row] = (int16_t)(fixed >> 16);
  mat->m[column].f[row] = fixed & 0xFFFF;
}

/**
 * Sets the position of a fixed-point matrix.
 * Note: that this will just set the values without any further checks/calculations.
 * @param mat matrix to be changed
 * @param pos position as a float[3]
 */
inline static void t3d_mat4fp_set_pos(T3DMat4FP *mat, const float pos[3]) {
  t3d_mat4fp_set_float(mat, 3, 0, pos[0]);
  t3d_mat4fp_set_float(mat, 3, 1, pos[1]);
  t3d_mat4fp_set_float(mat, 3, 2, pos[2]);
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
    {{1, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 1}, {0, 0, 0, 0}},
  }};
}
/**
 * Converts a float matrix to a fixed-point matrix.
 * @param matOut result
 * @param matIn input
 */
void t3d_mat4_to_fixed(T3DMat4FP *matOut, const T3DMat4 *matIn);

/**
 * Converts a float 4x4 matrix to a fixed-point 4x4 matrix.
 * The last row of the matrix is assumed to be {0,0,0,1}.
 * This should be preferred over 't3d_mat4_to_fixed' when possible as it is faster.
 * @param matOut result
 * @param matIn input
 */
void t3d_mat4_to_fixed_3x4(T3DMat4FP *matOut, const T3DMat4 *matIn);

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
 * Constructs an orthographic projection matrix
 * @param mat result
 * @param left
 * @param right
 * @param bottom
 * @param top
 * @param near near plane distance
 * @param far far plane distance
 */
void t3d_mat4_ortho(T3DMat4 *mat, float left, float right, float bottom, float top, float near, float far);

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

/**
 * Multiplies a 4x4 matrix with a 3D vector.
 * The W component of the vector is assumed to be 1.
 * @param vecOut result
 * @param mat matrix
 * @param vec input-vector
 */
inline static void t3d_mat4_mul_vec3(T3DVec4* vecOut, const T3DMat4 *mat, const T3DVec3* vec)
{
  for(uint32_t i=0; i<4; i++) {
    vecOut->v[i] = mat->m[0][i] * vec->v[0] +
                   mat->m[1][i] * vec->v[1] +
                   mat->m[2][i] * vec->v[2] +
                   mat->m[3][i];
  }
}

#ifdef __cplusplus
}
#endif

#endif //TINY3D_T3DMATH_H
