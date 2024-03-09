/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#ifndef TINY3D_T3D_H
#define TINY3D_T3D_H

#include <t3d/t3dmath.h>
#include <libdragon.h>

extern uint32_t T3D_RSP_ID;

enum T3DCmd {
  T3D_CMD_TRI_DRAW     = 0x0,
  T3D_CMD_SCREEN_SIZE  = 0x1,
  T3D_CMD_MAT_SET      = 0x2,
  T3D_CMD_MAT_READ     = 0x3,
  T3D_CMD_VERT_LOAD    = 0x4,
  T3D_CMD_LIGHT_SET    = 0x5,
  T3D_CMD_DRAWFLAGS    = 0x6,
  T3D_CMD_SET_CAMERA   = 0x7,
  T3D_CMD_PROJ_SET     = 0x8,
  T3D_CMD_LIGHT_COUNT  = 0x9,
  T3D_CMD_FOG_RANGE    = 0xA,
};

// Internal vertex format, interleaves two vertices
typedef struct {
  /* 0x00 */ int16_t posA[3]; // 16.0 fixed point
  /* 0x06 */ uint16_t normA;  // 5,5,5 packed normal
  /* 0x08 */ int16_t posB[3]; // 16.0 fixed point
  /* 0x0E */ uint16_t normB;  // 5,5,5 packed normal
  /* 0x10 */ uint32_t rgbaA; // RGBA8 color
  /* 0x14 */ uint32_t rgbaB; // RGBA8 color
  /* 0x18 */ int16_t stA[2]; // UV fixed point 10.5 (pixel coords)
  /* 0x1C */ int16_t stB[2]; // UV fixed point 10.5 (pixel coords)
} T3DVertPacked;

static_assert(sizeof(T3DVertPacked) == 0x20, "T3DVertPacked has wrong size");

enum T3DDrawFlags {
  T3D_FLAG_DEPTH      = 1 << 0,
  T3D_FLAG_TEXTURED   = 1 << 1,
  T3D_FLAG_SHADED     = 1 << 2,
  T3D_FLAG_CULL_FRONT = 1 << 3,
  T3D_FLAG_CULL_BACK  = 1 << 4,
};

/**
 * @brief Initializes the tiny3d library
 */
void t3d_init(void);

/**
 * @brief Destroys the tiny3d library
 */
void t3d_destroy(void);

void t3d_frame_start(void);

void t3d_screen_clear_color(color_t color);
void t3d_screen_clear_depth();

/**
 * Sets current screensize and guard-band/clipping settings
 * @param width width in pixels
 * @param height height in pixels
 * @param guardBandScale guard-band scale (1-4)
 * @param isReject if true, enables rejection, if false use clipping
 */
void t3d_screen_set_size(uint32_t width, uint32_t height, int guardBandScale, int isReject);

/**
 * Sets a new camera position and direction.
 * This will update the internal camera matrix and sends a command to the RSP.
 * @param eye camera position
 * @param target camera target/look-at
 */
__attribute__((unused))
void t3d_camera_look_at(const T3DVec3 *eye, const T3DVec3 *target);

const T3DMat4 *t3d_camera_get_matrix();

void t3d_projection_perspective(float fov, float near, float far);

const T3DMat4 *t3d_projection_get_matrix();

/**
 * @brief Draws a single triangle, referencing loaded vertices
 * @param draw_flags flags from 'T3DDrawFlags'
 * @param v0 vertex index 0
 * @param v1 vertex index 1
 * @param v2 vertex index 2
 */
void t3d_tri_draw(uint32_t v0, uint32_t v1, uint32_t v2);

void t3d_matrix_push(T3DMat4FP *mat, bool multiply);

/**
 * Directly loads a matrix into a slot, outside of the stack management.
 * (It will be multiplied with the projection matrix)
 * @param mat address to load matrix from
 * @param idxDst slot index (0-7)
 */
void t3d_mat_set(T3DMat4FP *mat, uint32_t idxDst);

/**
 * Multiplies a matrix with another matrix and loads it into a slot.
 * The matrix in 'idxMul' will be on the left-side of the multiplication.
 * @param mat address to load matrix from
 * @param idxDst slot index (0-7)
 * @param idxMul slot index of matrix to multiply with (0-7)
 */
void t3d_matrix_set_mul(T3DMat4FP *mat, uint32_t idxDst, uint32_t  idxMul);

/**
 * Sets the projection matrix
 * @param mat address to load matrix from
 */
void t3d_mat_proj_set(T3DMat4FP *mat);

void t3d_mat_read(void *mat);

void t3d_vert_load(const T3DVertPacked *vertices, uint32_t offsetSrc, uint32_t count);

void t3d_light_set_ambient(const uint8_t *color);

void t3d_light_set_directional(int index, const uint8_t *color, const T3DVec3 *dir);

static inline void t3d_light_set_count(int count) {
  rspq_write(T3D_RSP_ID, T3D_CMD_LIGHT_COUNT, count * 16);
}

void t3d_fog_set_range(float near, float far);

static inline void t3d_fog_disable() {
  t3d_fog_set_range(0.0f, 0.0f);
}

static inline void t3d_set_camera(const T3DVec3 *pos, const T3DVec3 *dir)
{
  int16_t posFP[3] = {
    (int16_t)(int32_t)(pos->v[0]),
    (int16_t)(int32_t)(pos->v[1]),
    (int16_t)(int32_t)(pos->v[2]),
  };

  uint8_t dirFP[3] = {
    (uint8_t)(int8_t)(dir->v[0] * 127.0f),
    (uint8_t)(int8_t)(dir->v[1] * 127.0f),
    (uint8_t)(int8_t)(dir->v[2] * 127.0f),
  };

  rspq_write(T3D_RSP_ID, T3D_CMD_SET_CAMERA,
    (dirFP[0] << 16) | (dirFP[1] << 8) | (dirFP[2] << 0),
    (posFP[0] << 16) | (posFP[1] << 0),
    posFP[2]
  );
}

uint16_t t3d_vert_pack_normal(const T3DVec3 *normal);

void t3d_state_set_drawflags(enum T3DDrawFlags drawFlags);

#endif //TINY3D_T3D_H
