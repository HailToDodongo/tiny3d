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
  T3D_CMD_SET_WORD     = 0x3,
  T3D_CMD_VERT_LOAD    = 0x4,
  T3D_CMD_LIGHT_SET    = 0x5,
  T3D_CMD_DRAWFLAGS    = 0x6,
  T3D_CMD_PROJ_SET     = 0x7,
  T3D_CMD_FOG_RANGE    = 0x8,
  T3D_CMD_FOG_STATE    = 0x9,
  T3D_CMD_TRI_SYNC     = 0xA,
  T3D_CMD_TRI_STRIP    = 0xB,
  //                   = 0xC,
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

_Static_assert(sizeof(T3DVertPacked) == 0x20, "T3DVertPacked has wrong size");

enum T3DDrawFlags {
  T3D_FLAG_DEPTH      = 1 << 0,
  T3D_FLAG_TEXTURED   = 1 << 1,
  T3D_FLAG_SHADED     = 1 << 2,
  T3D_FLAG_CULL_FRONT = 1 << 3,
  T3D_FLAG_CULL_BACK  = 1 << 4,
};

// Segment addresses, some are used internally but can be set by the user too
enum T3DSegment {
  T3D_SEGMENT_1 = 1,
  T3D_SEGMENT_2 = 2,
  T3D_SEGMENT_3 = 3,
  T3D_SEGMENT_4 = 4,
  T3D_SEGMENT_5 = 5,
  T3D_SEGMENT_6 = 6,
  T3D_SEGMENT_SKELETON = 7,
};

// Vertex effect functions
enum T3DVertexFX {
  T3D_VERTEX_FX_NONE           = 0,
  T3D_VERTEX_FX_SPHERICAL_UV   = 1,
  T3D_VERTEX_FX_CELSHADE_COLOR = 2,
  T3D_VERTEX_FX_CELSHADE_ALPHA = 3,
  T3D_VERTEX_FX_OUTLINE        = 4,
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

  T3DMat4 matCamProj; // combined view-projection matrix, calculated automatically on demand
  T3DFrustum viewFrustum; // frustum, calculated from the combined matrix
  bool _isCamProjDirty; // flag to indicate if the combined matrix needs to be recalculated

  int32_t offset[2];  // screen offset in pixel, [0,0] by default
  int32_t size[2];    // screen size in pixel, same as the allocated framebuffer size by default
  int guardBandScale; // guard band for clipping, 2 by default
  int useRejection;   // use rejection instead of clipping, false by default

  float _normScaleW; // factor to normalize W in the ucode, set automatically
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
    ._isCamProjDirty = true,
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
 * Returns the currently attached viewport.
 * @return viewport or NULL if none is attached
 */
T3DViewport *t3d_viewport_get();

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
 * Updates the projection matrix of the given viewport using a perspective projection.
 * The proj. matrix gets auto. applied at the next `t3d_viewport_attach` call.
 *
 * @param viewport
 * @param fov fov in radians
 * @param aspectRatio aspect ratio (= width / height)
 * @param near near plane distance
 * @param far far plane distance (should be >=40 to avoid depth-precision issues)
 */
void t3d_viewport_set_perspective(T3DViewport *viewport, float fov, float aspectRatio, float near, float far);

/**
 * Updates the projection matrix of the given viewport.
 * The proj. matrix gets auto. applied at the next `t3d_viewport_attach` call.
 *
 * NOTE: if you need a custom aspect ratio, use 't3d_viewport_set_perspective' instead.
 *
 * @param viewport
 * @param fov fov in radians
 * @param near near plane distance
 * @param far far plane distance (should be >=40 to avoid depth-precision issues)
 */
void t3d_viewport_set_projection(T3DViewport *viewport, float fov, float near, float far);

/**
 * Sets an orthographic projection matrix for the given viewport.
 * The matrix gets auto. applied at the next `t3d_viewport_attach` call.
 * @param viewport
 * @param left
 * @param right
 * @param bottom
 * @param top
 * @param near near plane distance
 * @param far far plane distance (should be >=40 to avoid depth-precision issues)
 */
void t3d_viewport_set_ortho(T3DViewport *viewport, float left, float right, float bottom, float top, float near, float far);

/**
 * Sets the normalization factor for W in the ucode.
 * NOTE: this gets called automatically by `t3d_viewport_set_projection`.
 * You only need to call this if you want to provide your own projection matrix.
 *
 * @param viewport
 * @param near
 * @param far
 */
inline static void t3d_viewport_set_w_normalize(T3DViewport *viewport, float near, float far) {
  viewport->_normScaleW = 2.0f / (far + near);
}

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
 * Calculates the view-space position of a given world-space position.
 * This will also handle offset viewports (e.g. for splitscreens)
 *
 * @param viewport viewport to calculate for
 * @param out output screen-space vector
 * @param pos input world-space position
 */
void t3d_viewport_calc_viewspace_pos(T3DViewport *viewport, T3DVec3 *out, const T3DVec3 *pos);

/**
 * @brief Draws a single triangle, referencing loaded vertices
 * @param draw_flags flags from 'T3DDrawFlags'
 * @param v0 vertex index 0
 * @param v1 vertex index 1
 * @param v2 vertex index 2
 */
void t3d_tri_draw(uint32_t v0, uint32_t v1, uint32_t v2);

/**
 * Draws a strip of triangles by loading an index buffer.
 * Note that this data must be in an internal format, so use 't3d_indexbuffer_convert' to convert it first.
 * The docs of 't3d_indexbuffer_convert' also describe the format of the input data.
 *
 * The data behind 'indexBuff' will be DMA'd by the ucode, so it must be aligned to 8 bytes,
 * and persist in memory until the triangles are drawn.
 * The target location of the DMA in DMEM is shared with the vertex cache, aligned to the end.
 * Make sure that there is enough space left to load the indices to not corrupt the vertices.
 * E.g. if you loaded 68 vertices, you have 2 slots free or 36*2 bytes, meaning you can load 36 indices.
 * Note that due to alignment reasons, a safety margin of 4 indices should be added if the free vertex count is odd.
 *
 * The built-in model format will use this function internally, if you plan on manually using it for model data,
 * check out 'tools/gltf_importer/src/optimizer/meshOptimizer.cpp' for an algorithm to do so.
 *
 * @param indexBuff index buffer to load
 * @param count amount of indices to load
 */
void t3d_tri_draw_strip(int16_t* indexBuff, int count);

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
 * You can set up to 7 directional lights, the amount can be set with 't3d_light_set_count'.
 * Note that directional and point lights share the same space.
 *
 * @param index index (0-6)
 * @param color color in RGBA8 format
 * @param dir direction vector
 */
void t3d_light_set_directional(int index, const uint8_t *color, const T3DVec3 *dir);

/**
 * Sets a point light.
 * You can set up to 7 point lights, the amount can be set with 't3d_light_set_count'.
 * Note that point and directional lights share the same space.
 *
 * The position is expected to be in world-space, and will be transformed internally.
 * Before calling this function, make sure to have a viewport attached.
 *
 * The size argument is a normalized distance, in the range 0.0 - 1.0.
 * Internally in the ucode, this maps to scaling factors in eye-space.
 * So unlike 'pos', it doesn't have any concrete units.
 *
 * @param index index (0-6)
 * @param color color in RGBA8 format
 * @param pos position in world-space
 * @param size distance, in range 0.0 - 1.0
 * @param ignoreNormals if true, the light will only check the distance, not the angle (useful for cutouts)
 */
void t3d_light_set_point(int index, const uint8_t *color, const T3DVec3 *pos, float size, bool ignoreNormals);

/**
 * Sets the amount of active lights (excl. ambient light).
 * Note that the ambient light does not count towards this limit and is always applied.
 * @param count amount of lights (0-6)
 */
void t3d_light_set_count(int count);

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
  // 0x06/0x08 are the offsets of attributes (color/UV) in a vertex on the RSP side
  // this allows the code to do a branch-less save
  rspq_write(T3D_RSP_ID, T3D_CMD_FOG_STATE, isEnabled ? 0x08 : 0x0C);
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

/**
 * Sets a function for vertex effects.
 * To disable it, set the function to 'T3D_VERTEX_FX_NONE'.
 * The arg0/arg1 values are stored ín DMEM and are used by the ucode.
 *
 * The meaning of those arguments depends on the type:
 * - T3D_VERTEX_FX_NONE          : (no arguments)
 * - T3D_VERTEX_FX_SPHERICAL_UV  : texture width/height
 * - T3D_VERTEX_FX_CELSHADE_COLOR: (no arguments)
 * - T3D_VERTEX_FX_CELSHADE_ALPHA: (no arguments)
 * - T3D_VERTEX_FX_OUTLINE       : pixel size X/Y
 *
 *
 * @param func vertex effect function
 * @param arg0 first argument
 * @param arg1 second argument
 */
void t3d_state_set_vertex_fx(enum T3DVertexFX func, int16_t arg0, int16_t arg1);

/**
 * Overrides the scale factor for some vertex effects.
 * This is currently only used by the 'T3D_VERTEX_FX_SPHERICAL_UV' function
 * to create an offset based on screen-space position.
 * This is automatically calculated by the ucode when setting the screen-size.
 * However if you see distortions near the edge of the screen, you can set this manually.
 * @param scale scale factor
 */
void t3d_state_set_vertex_fx_scale(uint16_t scale);

/**
 * Sets a new address in the segment table.
 * This acts ase a base-address for addresses in matrices/vertices
 * with a matching segment index.
 * Segment 0 is reserved and always has a zero value.
 * @param segmentId id (1-7)
 * @param address base RDRAM address
 */
void t3d_segment_set(uint8_t segmentId, void* address);

/**
 * Creates a dummy address to be used for vertex/matrix loads.
 * This will cause the address in the segment table to be used instead.
 * If you need relative addressing instead, use 't3d_segment_address'
 * @param segmentId id (1-7)
 * @return segmented address
 */
static inline void* t3d_segment_placeholder(uint8_t segmentId) {
  return (void*)(uint32_t)(segmentId << (8*3 + 2));
}

/**
 * Converts an address into a segmented one.
 * If used in a vertex/matrix load, it will cause the segment table to be used
 * and the address in there to be added on top.
 * To set entries in the segment table use 't3d_segment_set'.
 * @param segmentId id (1-7)
 * @param ptr pointer, can ber NULL for absolute addressing
 * @return segmented address
 */
static inline void* t3d_segment_address(uint8_t segmentId, void* ptr) {
  return (void*)(PhysicalAddr(ptr) | (segmentId << (8*3 + 2)));
}

// Index-buffer helpers:

/**
 * Converts an index buffer for triangle strips from indices to encoded DMEM pointers.
 * This is necessary in order for t3d_tri_draw_indexed to work.
 * Internally this data will be DMA'd by the ucode later on.
 *
 * Format:
 * The input data is expected to be an array of local indices (0-69) as s16 values.
 * The first 3 values define the initial triangle.
 * Every index afterwards will extend the strip by one triangle (winding order flipped).
 * Degenerate triangles are NOT supported, since it is always more efficient to use a restart flag.
 * To start a new strip, set the MSB of the next index, so: 'idx | (1<<15)'
 * meaning the flagged value and the 2 following values will form a new triangle, re-starting the strip.
 * Since restarting does not need a special index value, non-stripped indices (triangle lists)
 * can be encoded too without overhead.
 *
 * Examples:   (original triangles -> expected input of this function)
 * [4,5,6] [0,1,2] [7,8,9]         -> [4,5,6,#1,1,2,#8,8,9]
 * [0,1,2] [3,0,2] [0,3,4] [5,0,4] -> [1,2,0,3,0,4,5]
 * (The '#' is used to mark the MSB here)
 *
 * @param indices index buffer of local indices, will be modified in-place
 * @param count index count
 */
void t3d_indexbuffer_convert(int16_t indices[], int count);

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

// C++ wrappers that allow (const-)references to be passed instead of pointers
#ifdef __cplusplus

inline void t3d_viewport_attach(T3DViewport &viewport) { t3d_viewport_attach(&viewport); }
inline void t3d_viewport_set_area(T3DViewport &viewport, int32_t x, int32_t y, int32_t width, int32_t height) { t3d_viewport_set_area(&viewport, x, y, width, height); }
inline void t3d_viewport_set_projection(T3DViewport &viewport, float fov, float near, float far) { t3d_viewport_set_projection(&viewport, fov, near, far); }
inline void t3d_viewport_set_ortho(T3DViewport &viewport, float left, float right, float bottom, float top, float near, float far) { t3d_viewport_set_ortho(&viewport, left, right, bottom, top, near, far); }
inline void t3d_viewport_set_w_normalize(T3DViewport &viewport, float near, float far) { t3d_viewport_set_w_normalize(&viewport, near, far); }
inline void t3d_viewport_look_at(T3DViewport &viewport, const T3DVec3 &eye, const T3DVec3 &target, const T3DVec3 &up = T3DVec3{{0,1,0}}) { t3d_viewport_look_at(&viewport, &eye, &target, &up); }
inline void t3d_viewport_calc_viewspace_pos(T3DViewport &viewport, T3DVec3 &out, const T3DVec3 &pos) { t3d_viewport_calc_viewspace_pos(&viewport, &out, &pos); }

inline void t3d_light_set_directional(int index, const uint8_t *color, const T3DVec3 &dir) { t3d_light_set_directional(index, color, &dir); }
inline void t3d_light_set_point(int index, const uint8_t *color, const T3DVec3 &pos, float size, bool ignoreNormals = false) { t3d_light_set_point(index, color, &pos, size, ignoreNormals); }

inline void t3d_light_set_ambient(const color_t &color) { t3d_light_set_ambient((const uint8_t*)&color); }
inline void t3d_light_set_directional(int index, const color_t &color, const T3DVec3 &dir) { t3d_light_set_directional(index, (const uint8_t*)&color, &dir); }
inline void t3d_light_set_point(int index, const color_t &color, const T3DVec3 &pos, float size, bool ignoreNormals = false) { t3d_light_set_point(index, (const uint8_t*)&color, &pos, size, ignoreNormals); }

#endif

#endif //TINY3D_T3D_H
