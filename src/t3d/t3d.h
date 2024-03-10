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
  T3D_CMD_DEBUG_READ   = 0x3,
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

/**
 * @brief Starts a new frame, this will setup some default states
 */
void t3d_frame_start(void);

/// @brief Clears the screen with a given color
void t3d_screen_clear_color(color_t color);

/// @brief Clears the depth buffer with a fixed value (0xFFFC)
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

/// @brief Returns the current camera matrix (floating-point)
const T3DMat4 *t3d_camera_get_matrix();

/**
 * Constructs and sets a perspective projection matrix.
 * @param fov fov in radians
 * @param near near plane distance
 * @param far far plane distance
 */
void t3d_projection_perspective(float fov, float near, float far);

/// @brief Returns the current projection matrix (floating-point)
const T3DMat4 *t3d_projection_get_matrix();

/**
 * @brief Draws a single triangle, referencing loaded vertices
 * @param draw_flags flags from 'T3DDrawFlags'
 * @param v0 vertex index 0
 * @param v1 vertex index 1
 * @param v2 vertex index 2
 */
void t3d_tri_draw(uint32_t v0, uint32_t v1, uint32_t v2);

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
void t3d_matrix_set_proj(T3DMat4FP *mat);

/**
 * Loads a vertex buffer with a given size, this can then be used to draw triangles.
 *
 * @param vertices vertex buffer
 * @param count how many vertices to load (2-64), must be multiple of 2!
 */
void t3d_vert_load(const T3DVertPacked *vertices, uint32_t count);

/**
 * Sets the global ambient light color.
 * This color is always active and applied to all objects.
 * To disable the ambient light, set the color to black.
 * @param color color in RGBA8 format
 */
void t3d_light_set_ambient(const uint8_t *color);

/**
 * Sets a directional light.
 * You can set up to 8 directional lights, the amount can be set with 't3d_light_set_count'.
 *
 * @param index index (0-7)
 * @param color color in RGBA8 format
 * @param dir direction vector
 */
void t3d_light_set_directional(int index, const uint8_t *color, const T3DVec3 *dir);

/**
 * Sets the amount of active lights (excl. ambient light).
 * @param count amount of lights (0-7)
 */
static inline void t3d_light_set_count(int count) {
  rspq_write(T3D_RSP_ID, T3D_CMD_LIGHT_COUNT, count * 16);
}

/**
 * Sets the range of the fog.
 * To disable fog, use 't3d_fog_disable' or set 'near' and 'far' to 0.
 * Note: in order for fog itself to work, make sure to setup the needed RSPQ commands.
 *
 * @param near start of the fog effect
 * @param far end of the fog effect
 */
void t3d_fog_set_range(float near, float far);

/// @brief Disables fog
static inline void t3d_fog_disable() {
  t3d_fog_set_range(0.0f, 0.0f);
}

/**
 * Sets up a matrix for the camera from a given pos/direction.
 * This matrix is then also applied as the current view matrix.
 *
 * @param pos world-space camera position
 * @param dir world-space camera direction
 */
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

/**
 * Packs a floating-point normal into the internal 5.6.5 format.
 * @param normal normal vector
 * @return packed normal
 */
uint16_t t3d_vert_pack_normal(const T3DVec3 *normal);

/**
 * Sets various draw flags, this will affect how triangles are drawn.
 * @param drawFlags
 */
void t3d_state_set_drawflags(enum T3DDrawFlags drawFlags);

// Vertex-buffer helpers:

/**
 * Returns the pointer to a position of a vertex in a buffer
 * @param vert vertex buffer
 * @param idx vertex index
 */
static inline int16_t* t3d_vertbuffer_get_pos(T3DVertPacked vert[], int idx) {
  return (idx & 1) ? vert[idx/2].posB : vert[idx/2].posA;
}

/**
 * Returns the pointer to the UV of a vertex in a buffer
 * @param vert vertex buffer
 * @param idx vertex index
 */
static inline int16_t* t3d_vertbuffer_get_uv(T3DVertPacked vert[], int idx) {
  return (idx & 1) ? vert[idx/2].stB : vert[idx/2].stA;
}

/**
 * Returns the pointer to the color (as a u32) of a vertex in a buffer
 * @param vert vertex buffer
 * @param idx vertex index
 */
static inline uint32_t* t3d_vertbuffer_get_color(T3DVertPacked vert[], int idx) {
  return (idx & 1) ? &vert[idx/2].rgbaB : &vert[idx/2].rgbaA;
}

/**
 * Returns the pointer to the color (as a u8[4]) of a vertex in a buffer
 * @param vert vertex buffer
 * @param idx vertex index
 */
static inline uint8_t* t3d_vertbuffer_get_rgba(T3DVertPacked vert[], int idx) {
  return (idx & 1) ? (uint8_t*)&vert[idx/2].rgbaB : (uint8_t*)&vert[idx/2].rgbaA;
}

/**
 * Returns the pointer to the packed normal of a vertex in a buffer
 * @param vert vertex buffer
 * @param idx vertex index
 */
static inline uint16_t* t3d_vertbuffer_get_norm(T3DVertPacked vert[], int idx) {
  return (idx & 1) ? &vert[idx/2].normB : &vert[idx/2].normA;
}

#endif //TINY3D_T3D_H
