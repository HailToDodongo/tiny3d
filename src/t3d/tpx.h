/**
* @copyright 2024 - Max Bebök
* @license MIT
* @file tpx.h
*/
#ifndef TINYPX_PTX_H
#define TINYPX_PTX_H

#include <libdragon.h>
#include <t3d/t3dmath.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern uint32_t TPX_RSP_ID;

// RSP commands, must match with the commands defined in `rsp/rsp_tinypx.rspl`
enum TPXCmd {
  TPX_CMD_SYNC_T3D     = 0x0,
  TPX_CMD_DRAW_COLOR   = 0x1,
  TPX_CMD_MATRIX_STACK = 0x2,
  TPX_CMD_SET_DMEM     = 0x3,
  TPX_CMD_DRAW_TEXTURE = 0x4,
  //                   = 0x5,
  //                   = 0x6,
  //                   = 0x7,
  //                   = 0x8,
  //                   = 0x9,
  //                   = 0xA,
  //                   = 0xB,
  //                   = 0xC,
  //                   = 0xD,
  //                   = 0xE,
  //                   = 0xF,
};

typedef struct {
   // Internal matrix stack size, must be at least 2.
   // If set two zero, 4 will be used by default.
  int matrixStackSize;
} TPXInitParams;

typedef struct {
  int8_t posA[3];
  int8_t sizeA;
  int8_t posB[3];
  int8_t sizeB;
  uint8_t colorA[4];
  uint8_t colorB[4];
}  __attribute__((packed, aligned(16))) TPXParticleS8;

static_assert(sizeof(TPXParticleS8) == 16, "TPXParticleS8 size mismatch");

/**
 * @deprecated Use 'TPXParticleS8' instead.
 */
[[deprecated("Use 'TPXParticleS8' instead")]] typedef TPXParticleS8 TPXParticle;

typedef struct {
  int16_t posA[3];
  int8_t sizeA;
  uint8_t texOffsetA;
  int16_t posB[3];
  int8_t sizeB;
  uint8_t texOffsetB;
  uint8_t colorA[4];
  uint8_t colorB[4];
} __attribute__((packed, aligned(8))) TPXParticleS16;

static_assert(sizeof(TPXParticleS16) == 24, "TPXParticle16 size mismatch");

/**
 * @brief Initializes the tinyPX library
 * @param params settings to configure the library
 */
void tpx_init(TPXInitParams params);

/**
 * T3D and TPX have separate states and ucodes.
 * This function copies the relevant T3D state to TPX.
 *
 * Settings that are copied:
 *  - Screen size
 *  - Current MVP matrix
 *  - W-normalization factor
 */
void tpx_state_from_t3d();

/**
 * Sets a global scaling factor applied to all particles
 * Note that you can only scale down, not up.
 * @param scaleX horizontal scaling factor, range: 0.0 - 1.0
 * @param scaleY vertical scaling factor, range: 0.0 - 1.0
 */
void tpx_state_set_scale(float scaleX, float scaleY);

/**
 * Sets a global base size from which all particles are scaled.
 * This values can later only be scaled down, not up.
 * By default it is set to 128, which should be good enough for most cases.
 * If you notices particles are way too small or large, you can change this value.
 * This may happen if you use large or very small screen sizes, or unusual near/far values.
 * @param baseSize unit-less base size, def.: 128
 */
void tpx_state_set_base_size(uint16_t baseSize);

/**
 * Sets global params for textured particles.
 * Those can be used to animate particles over time.
 * NOTE: Check out the '19_particles_tex' example for more info.
 *
 * @param offsetX base-offset UV offset in 1/4th pixels.<br>
 *                This refers to the 8x8px base size irrespective of the actual texture size.
 * @param mirrorPoint point where the texture should be mirrored<br>
 *                This is in amount of sections of the 8x8px base size.<br>
 *                So if you want to repeat after 32px set it to 4.<br>
 *                If you want no mirroring, set it to 0.
 */
void tpx_state_set_tex_params(int16_t offsetX, uint16_t mirrorPoint);

/**
 * Draws a given amount of particles (8bit position precision).
 * In contrast to triangles in t3d, this works in a single command.
 * So load, transform and draw happens in one go.
 * @param particles pointer to the particle data
 * @param count number of particles to draw
 */
void tpx_particle_draw_s8(TPXParticleS8 *particles, uint32_t count);

[[deprecated("Use 'tpx_particle_draw_s8' instead")]]
inline static void tpx_particle_draw(TPXParticleS8 *particles, uint32_t count) {
  return tpx_particle_draw_s8(particles, count);
}

/**
 * Draws a given amount of particles (16bit position precision).
 * 16bit Precision gives you larger range but comes with slightly more memory and runtime cost.
 * Whenever possible use the 8bit version instead.
 * It is most useful if you need to cover large ranges, e.g. when using it for billboards in scene.
 *
 * In contrast to triangles in t3d, this works in a single command.
 * So load, transform and draw happens in one go.
 * @param particles pointer to the particle data
 * @param count number of particles to draw
 */
void tpx_particle_draw_s16(TPXParticleS16 *particles, uint32_t count);

/**
 * Draws a given amount of particles with a texture.
 * In contrast to triangles in t3d, this works in a single command.
 * So load, transform and draw happens in one go.
 *
 * Note: this expects that you already setup textures.
 * It will also always use TILE0 for the rect-commands.
 * The colors alpha channel acts as a texture offset.
 *
 * @param particles pointer to the particle data
 * @param count number of particles to draw
 */
void tpx_particle_draw_tex_s8(TPXParticleS8 *particles, uint32_t count);

[[deprecated("Use 'tpx_particle_draw_tex_s8' instead")]]
inline static void tpx_particle_draw_tex(TPXParticleS8 *particles, uint32_t count) {
  return tpx_particle_draw_tex_s8(particles, count);
}

/**
 * Draws a given amount of particles (16bit position precision).
 * 16bit Precision gives you larger range but comes with slightly more memory and runtime cost.
 * Whenever possible use the 8bit version instead.
 * It is most useful if you need to cover large ranges, e.g. when using it for billboards in scene.
 *
 * Note: this expects that you already setup textures.
 * It will also always use TILE0 for the rect-commands.
 * A per-particle texture offset can be set in 'texOffsetA'/'texOffsetB'.
 *
 * @param particles pointer to the particle data
 * @param count number of particles to draw
 */
void tpx_particle_draw_tex_s16(TPXParticleS16 *particles, uint32_t count);

/**
 * Directly loads a matrix, overwriting the current stack position.
 *
 * With 'doMultiply' set to false, this lets you completely replace the current matrix.
 * With 'doMultiply' set to true, it serves as a faster version of a pop+push combination.
 *
 * @param mat address to load matrix from
 * @param doMultiply if true, the matrix will be multiplied with the previous stack entry
 */
void tpx_matrix_set(const T3DMat4FP *mat, bool doMultiply);

/**
 * Multiplies a matrix with the current stack position, then pushes it onto the stack.
 * @param mat address to load matrix from
 */
void tpx_matrix_push(const T3DMat4FP *mat);

/**
 * Pops the current matrix from the stack.
 * @param count how many matrices to pop
 */
void tpx_matrix_pop(int count);

/**
 * Moves the stack pos. without changing a matrix or causing re-calculations.
 * This should only be used in preparation for 'tpx_matrix_set' calls.
 *
 * E.g. instead of multiple push/pop combis, use:
 * - 'tpx_matrix_push_pos(1)' once
 * - then multiple 'tpx_matrix_set'
 * - finish with 'tpx_matrix_pop'
 *
 * This is the most efficient way to set multiple matrices at the same level.
 * @param count relative change (matrix count), should usually be 1
 */
void tpx_matrix_push_pos(int count);

/**
 * Returns the pointer to a position of a particle in a buffer
 * @param vert particle buffer
 * @param idx particle index
 */
static inline int8_t* tpx_buffer_s8_get_pos(TPXParticleS8 pt[], int idx) {
  return (idx & 1) ? pt[idx/2].posB : pt[idx/2].posA;
}

[[deprecated("Use 'tpx_buffer_s8_get_pos' instead")]]
static inline int8_t* tpx_buffer_get_pos(TPXParticleS8 pt[], int idx) {
  return tpx_buffer_s8_get_pos(pt, idx);
}

/**
 * Returns the pointer to the size of a particle in a buffer
 * @param pt particle buffer
 * @param idx particle index
 */
static inline int8_t* tpx_buffer_s8_get_size(TPXParticleS8 pt[], int idx) {
  return (idx & 1) ? &pt[idx/2].sizeB : &pt[idx/2].sizeA;
}

[[deprecated("Use 'tpx_buffer_s8_get_size' instead")]]
static inline int8_t* tpx_buffer_get_size(TPXParticleS8 pt[], int idx) {
  return tpx_buffer_s8_get_size(pt, idx);
}

/**
 * Returns the pointer to the color (as a u32) of a particle in a buffer
 * @param pt particle buffer
 * @param idx particle index
 */
static inline uint32_t* tpx_buffer_s8_get_color(TPXParticleS8 pt[], int idx) {
  return (idx & 1) ? (uint32_t*)&pt[idx/2].colorB : (uint32_t*)&pt[idx/2].colorA;
}

[[deprecated("Use 'tpx_buffer_s8_get_color' instead")]]
static inline uint32_t* tpx_buffer_get_color(TPXParticleS8 pt[], int idx) {
  return tpx_buffer_s8_get_color(pt, idx);
}

/**
 * Returns the pointer to the color (as a u8[4]) of a particle in a buffer
 * @param pt particle buffer
 * @param idx particle index
 */
static inline uint8_t* tpx_buffer_s8_get_rgba(TPXParticleS8 pt[], int idx) {
  return (idx & 1) ? pt[idx/2].colorB : pt[idx/2].colorA;
}

[[deprecated("Use 'tpx_buffer_s8_get_rgba' instead")]]
static inline uint8_t* tpx_buffer_get_rgba(TPXParticleS8 pt[], int idx) {
  return tpx_buffer_s8_get_rgba(pt, idx);
}

/**
 * Returns the pointer to a position of a particle in a buffer
 * @param vert particle buffer
 * @param idx particle index
 */
static inline int16_t* tpx_buffer_s16_get_pos(TPXParticleS16 pt[], int idx) {
  return (idx & 1) ? pt[idx/2].posB : pt[idx/2].posA;
}

/**
 * Returns the pointer to the size of a particle in a buffer
 * @param pt particle buffer
 * @param idx particle index
 */
static inline int8_t* tpx_buffer_s16_get_size(TPXParticleS16 pt[], int idx) {
  return (idx & 1) ? &pt[idx/2].sizeB : &pt[idx/2].sizeA;
}

/**
 * Returns the pointer to the color (as a u32) of a particle in a buffer
 * @param pt particle buffer
 * @param idx particle index
 */
static inline uint8_t* tpx_buffer_s16_get_rgba(TPXParticleS16 pt[], int idx) {
  return (idx & 1) ? pt[idx/2].colorB : pt[idx/2].colorA;
}

/**
 * Returns the pointer to the texture offset in the buffer.
 * This is only present in the 16bit buffer, in the 8bit version this stored in alpha channel.
 * @param pt particle buffer
 * @param idx particle index
*/
static inline uint8_t* tpx_buffer_s16_get_tex_offset(TPXParticleS16 pt[], int idx) {
  return (idx & 1) ? &pt[idx/2].texOffsetA : &pt[idx/2].texOffsetB;
}

/**
 * Swaps two particles in a buffer
 * @param pt buffer to swap particles in
 * @param idxA index of the first particle
 * @param idxB index of the second particle
 */
void tpx_buffer_s8_swap(TPXParticleS8 pt[], uint32_t idxA, uint32_t idxB);

[[deprecated("Use 'tpx_buffer_s8_swap' instead")]]
static inline void tpx_buffer_swap(TPXParticleS8 pt[], uint32_t idxA, uint32_t idxB) {
  tpx_buffer_s8_swap(pt, idxA, idxB);
}

/**
 * Swaps two particles in a buffer
 * @param pt buffer to swap particles in
 * @param idxA index of the first particle
 * @param idxB index of the second particle
 */
void tpx_buffer_s16_swap(TPXParticleS16 pt[], uint32_t idxA, uint32_t idxB);

/**
 * Copies a particle into another place in a buffer
 * This will overwrite the destination particle and keep the source particle unchanged.
 * @param pt buffer to copy particles in
 * @param idxDst destination index
 * @param idxSrc source index
 */
void tpx_buffer_s8_copy(TPXParticleS8 pt[], uint32_t idxDst, uint32_t idxSrc);

[[deprecated("Use 'tpx_buffer_s8_copy' instead")]]
static inline void tpx_buffer_copy(TPXParticleS8 pt[], uint32_t idxDst, uint32_t idxSrc) {
  tpx_buffer_s8_copy(pt, idxDst, idxSrc);
}

/**
 * Copies a particle into another place in a buffer
 * This will overwrite the destination particle and keep the source particle unchanged.
 * @param pt buffer to copy particles in
 * @param idxDst destination index
 * @param idxSrc source index
 */
void tpx_buffer_s16_copy(TPXParticleS16 pt[], uint32_t idxDst, uint32_t idxSrc);

/**
 * Destroys the tinyPX library and frees all resources
 */
void tpx_destroy();

#ifdef __cplusplus
}
#endif

#endif // TINYPX_PTX_H