/**
* @copyright 2024 - Max Bebök
* @license MIT
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
  //                   = 0x4,
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
}  __attribute__((packed, aligned(16))) TPXParticle;

_Static_assert(sizeof(TPXParticle) == 16, "TPXParticle size mismatch");

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
 * Draws a given amount of particles.
 * In contrast to triangles in t3d, this works in a single command.
 * So load, transform and draw happens in one go.
 * @param particles pointer to the particle data
 * @param count number of particles to draw
 */
void tpx_particle_draw(TPXParticle *particles, uint32_t count);

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
static inline int8_t* tpx_buffer_get_pos(TPXParticle pt[], int idx) {
  return (idx & 1) ? pt[idx/2].posB : pt[idx/2].posA;
}

/**
 * Returns the pointer to the size of a particle in a buffer
 * @param pt particle buffer
 * @param idx particle index
 */
static inline int8_t* tpx_buffer_get_size(TPXParticle pt[], int idx) {
  return (idx & 1) ? &pt[idx/2].sizeB : &pt[idx/2].sizeA;
}

/**
 * Returns the pointer to the color (as a u32) of a particle in a buffer
 * @param pt particle buffer
 * @param idx particle index
 */
static inline uint32_t* tpx_buffer_get_color(TPXParticle pt[], int idx) {
  return (idx & 1) ? (uint32_t*)&pt[idx/2].colorB : (uint32_t*)&pt[idx/2].colorA;
}

/**
 * Returns the pointer to the color (as a u8[4]) of a particle in a buffer
 * @param pt particle buffer
 * @param idx particle index
 */
static inline uint8_t* tpx_buffer_get_rgba(TPXParticle pt[], int idx) {
  return (idx & 1) ? pt[idx/2].colorB : pt[idx/2].colorA;
}

/**
 * Swaps two particles in a buffer
 * @param pt buffer to swap particles in
 * @param idxA index of the first particle
 * @param idxB index of the second particle
 */
void tpx_buffer_swap(TPXParticle pt[], uint32_t idxA, uint32_t idxB);

/**
 * Copies a particle into another place in a buffer
 * This will overwrite the destination particle and keep the source particle unchanged.
 * @param pt buffer to copy particles in
 * @param idxDst destination index
 * @param idxSrc source index
 */
void tpx_buffer_copy(TPXParticle pt[], uint32_t idxDst, uint32_t idxSrc);

/**
 * Destroys the tinyPX library and frees all resources
 */
void tpx_destroy();

#ifdef __cplusplus
}
#endif

#endif // TINYPX_PTX_H