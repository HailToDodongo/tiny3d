/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#ifndef TINY3D_T3D_H
#define TINY3D_T3D_H

#include <t3d/t3dmath.h>
#include <libdragon.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern uint32_t T3D_RSP_ID;

// RSP commands, must match with the commands defined in `rsp/rsp_tiny3d.rspl`
enum T3DCmd {
  T3D_CMD_TRI_DRAW     = 0x0,
  T3D_CMD_SCREEN_SIZE  = 0x1,
  T3D_CMD_MATRIX_STACK = 0x2,
  T3D_CMD_DEBUG_READ   = 0x3,
  T3D_CMD_VERT_LOAD    = 0x4,
  T3D_CMD_LIGHT_SET    = 0x5,
  T3D_CMD_DRAWFLAGS    = 0x6,
  T3D_CMD_SET_CAMERA   = 0x7,
  T3D_CMD_PROJ_SET     = 0x8,
  T3D_CMD_LIGHT_COUNT  = 0x9,
  T3D_CMD_FOG_RANGE    = 0xA,
  T3D_CMD_FOG_STATE    = 0xB,
  T3D_CMD_TRI_SYNC     = 0xC,
  //                   = 0xD,
  //                   = 0xE,
  //                   = 0xF,
};

// Internal vertex format, interleaves two vertices
typedef struct {
  /* 0x00 */ int16_t posA[3]; // s16 (used in the ucode as the int. part of a s16.16)
  /* 0x06 */ uint16_t normA;  // 5,5,5 packed normal
  /* 0x08 */ int16_t posB[3]; // s16 (used in the ucode as the int. part of a s16.16)
  /* 0x0E */ uint16_t normB;  // 5,6,5 packed normal
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
 * Viewport, combines several settings which are usually used together.
 * This includes the screen-rect, camera-matrix and projection-matrix.
 *
 * Note: members considered private are prefixed with an underscore.
 */
typedef struct {
  T3DMat4FP _matCameraFP; // calculated from 'matCamera' by 't3d_viewport_look_at'
  T3DMat4FP _matProjFP;   // calculated from 'matProj' by 't3d_viewport_set_projection'

  T3DMat4 matCamera; // view matrix, can be set via `t3d_viewport_look_at`
  T3DMat4 matProj;   // projection matrix, can be set via `t3d_viewport_set_projection`

  int32_t offset[2];  // screen offset in pixel, [0,0] by default
  int32_t size[2];    // screen size in pixel, same as the allocated framebuffer size by default
  int guardBandScale; // guard band for clipping, 2 by default
  int useRejection;   // use rejection instead of clipping, false by default
} T3DViewport;

/**
 * Settings to configure t3d during initialization.
 */
typedef struct {
   // Internal matrix stack size, must be at least 2.
   // If set two zero, 8 will be used by default.
  int matrixStackSize;
} T3DInitParams;

/**
 * @brief Initializes the tiny3d library
 * @param params settings to configure the library
 */
void t3d_init(T3DInitParams params);

/**
 * @brief Destroys the tiny3d library
 */
void t3d_destroy(void);

/**
 * @brief Starts a new frame, this will setup some default states
 */
void t3d_frame_start(void);


/// @brief Clears the entire screen with a given color
void t3d_screen_clear_color(color_t color);

/// @brief Clears the entire depth buffer with a fixed value (0xFFFC)
void t3d_screen_clear_depth();

/**
 * Creates a viewport struct, this only creates a struct and doesn't change any setting.
 * Note: Nothing in this struct needs any cleanup/free.
 *
 * @return struct with the documented default values
 */
inline static T3DViewport t3d_viewport_create() {
  return (T3DViewport){
    .offset = {0, 0},
    .size = {(int32_t)display_get_width(), (int32_t)display_get_height()},
    .guardBandScale = 2,
    .useRejection = false,
  };
}

/**
 * Uses the given viewport for further rendering and applies its settings.
 * This will set the visible screen-rect, and apply the camera and projection matrix.
 *
 * Note that you may have to re-apply directional lights if you are using multiple viewports,
 * as they are depend on the view matrix.
 *
 * @param viewport viewport, pointer must be valid until the next `t3d_viewport_attach` call
 */
void t3d_viewport_attach(T3DViewport *viewport);

/**
 * Convenience function to set the area of a viewport.
 * @param viewport
 * @param x position (0 by default)
 * @param y
 * @param width size (display width by default)
 * @param height
 */
inline static void t3d_viewport_set_area(T3DViewport *viewport, int32_t x, int32_t y, int32_t width, int32_t height) {
  viewport->offset[0] = x;
  viewport->offset[1] = y;
  viewport->size[0] = width;
  viewport->size[1] = height;
}

/**
 * Updates the projection matrix of the given viewport.
 * The proj. matrix gets auto. applied at the next `t3d_viewport_attach` call.
 *
 * @param viewport
 * @param fov fov in radians
 * @param near near plane distance
 * @param far far plane distance
 */
void t3d_viewport_set_projection(T3DViewport *viewport, float fov, float near, float far);

/**
 * Sets a new camera position and direction for the given viewport.
 * The view matrix gets auto. applied at the next `t3d_viewport_attach` call.
 *
 * @param eye camera position
 * @param target camera target/look-at
 * @param up camera up vector, expected to be {0,1,0} by default
 */
void t3d_viewport_look_at(T3DViewport *viewport, const T3DVec3 *eye, const T3DVec3 *target, const T3DVec3 *up);

/**
 * @brief Draws a single triangle, referencing loaded vertices
 * @param draw_flags flags from 'T3DDrawFlags'
 * @param v0 vertex index 0
 * @param v1 vertex index 1
 * @param v2 vertex index 2
 */
void t3d_tri_draw(uint32_t v0, uint32_t v1, uint32_t v2);

/**
 * Syncs pending triangles.
 * This needs to be called after triangles where drawn and a different overlay
 * which also generates RDP commands will be used. (e.g. RDPQ)
 */
inline static void t3d_tri_sync() {
  rspq_write(T3D_RSP_ID, T3D_CMD_TRI_SYNC, 0);
}

/**
 * Directly loads a matrix, overwriting the current stack position.
 *
 * With 'doMultiply' set to false, this lets you completely replace the current matrix.
 * With 'doMultiply' set to true, it serves as a faster version of a pop+push combination.
 *
 * @param mat address to load matrix from
 * @param doMultiply if true, the matrix will be multiplied with the previous stack entry
 */
void t3d_matrix_set(const T3DMat4FP *mat, bool doMultiply);

/**
 * Multiplies a matrix with the current stack position, then pushes it onto the stack.
 * @param mat address to load matrix from
 */
void t3d_matrix_push(const T3DMat4FP *mat);

/**
 * Pops the current matrix from the stack.
 * @param count how many matrices to pop
 */
void t3d_matrix_pop(int count);

/**
 * Moves the stack pos. without changing a matrix or causing re-calculations.
 * This should only be used in preparation for 't3d_matrix_set' calls.
 *
 * E.g. instead of multiple push/pop combis, use:
 * - 't3d_matrix_push_pos(1)' once
 * - then multiple 't3d_matrix_set'
 * - finish with 't3d_matrix_pop'
 *
 * This is the most efficient way to set multiple matrices at the same level.
 * @param count relative change (matrix count), should usually be 1
 */
void t3d_matrix_push_pos(int count);

/**
 * Sets the projection matrix, this is stored outside of the matrix stack.
 * @param mat address to load matrix from
 */
void t3d_matrix_set_proj(const T3DMat4FP *mat);

/**
 * Loads a vertex buffer with a given size, this can then be used to draw triangles.
 *
 * @param vertices vertex buffer
 * @param offset offset in the target buffer (0-62)
 * @param count how many vertices to load (2-64), must be multiple of 2!
 */
void t3d_vert_load(const T3DVertPacked *vertices, uint32_t offset, uint32_t count);

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

/**
 * Enables or disables fog, this can be set independently from the range.
 * @param isEnabled
 */
static inline void t3d_fog_set_enabled(bool isEnabled) {
  rspq_write(T3D_RSP_ID, T3D_CMD_FOG_STATE, (uint8_t)isEnabled);
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

#ifdef __cplusplus
}
#endif

#endif //TINY3D_T3D_H
