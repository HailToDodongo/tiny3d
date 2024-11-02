/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#ifndef TINYPX_PTX_H
#define TINYPX_PTX_H

#include <libdragon.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern uint32_t TPX_RSP_ID;

// RSP commands, must match with the commands defined in `rsp/rsp_tinypx.rspl`
enum TPXCmd {
  TPX_CMD_SYNC_T3D   = 0x0,
  TPX_CMD_DRAW_COLOR = 0x1,
  TPX_CMD_DRAW_TEX   = 0x2,
  TPX_CMD_SET_DMEM   = 0x3,
  //                 = 0x4,
  //                 = 0x5,
  //                 = 0x6,
  //                 = 0x7,
  //                 = 0x8,
  //                 = 0x9,
  //                 = 0xA,
  //                 = 0xB,
  //                 = 0xC,
  //                 = 0xD,
  //                 = 0xE,
  //                 = 0xF,
};

typedef struct {
  // nothing here yet
} TPXInitParams;

typedef struct {
  int8_t posA[3];
  int8_t sizeA;
  int8_t posB[3];
  int8_t sizeB;
  uint8_t colorA[4];
  uint8_t colorB[4];
}  __attribute__((packed, aligned(__alignof__(uint32_t)))) TPXParticle;

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
 * @param scale scaling factor, range: 0.0 - 1.0
 */
void tpx_state_set_scale(float scale);

/**
 * Draws a given amount of particles.
 * In contrast to triangles in t3d, this works in a single command.
 * So load, transform and draw happens in one go.
 * @param particles pointer to the particle data
 * @param count number of particles to draw
 */
void tpx_particle_draw(TPXParticle *particles, uint32_t count);

/**
 * Destroys the tinyPX library and frees all resources
 */
void tpx_destroy();

#ifdef __cplusplus
}
#endif

#endif // TINYPX_PTX_H