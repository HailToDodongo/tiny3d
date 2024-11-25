/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include <t3d/tpx.h>
#include <t3d/t3d.h>

#include "rsp/rsp_tiny3d.h"
#include "rsp/rsp_tinypx.h"

extern rsp_ucode_t rsp_tiny3d;
DEFINE_RSP_UCODE(rsp_tinypx);
uint32_t TPX_RSP_ID = 0;

#define SWAP_U32(a, b) {uint32_t tmp = a; a = b; b = tmp;}
#define MAX_PARTICLES_COLOR 344

static T3DMat4FP *matrixStack = NULL;

void tpx_init([[maybe_unused]] TPXInitParams params)
{
  if(params.matrixStackSize <= 0)params.matrixStackSize = 8;

  rspq_init();
  matrixStack = malloc_uncached(sizeof(T3DMat4FP) * params.matrixStackSize);
  t3d_mat4fp_identity(&matrixStack[0]);

  char* state = (char*)UncachedAddr(rspq_overlay_get_state(&rsp_tiny3d));

  uint32_t *stackPtr = (uint32_t*)((char*)state + ((RSP_TPX_MATRIX_STACK_PTR - RSP_TPX_STATE_MEM_START) & 0xFFFF));
  *stackPtr = (uint32_t)UncachedAddr(matrixStack);

  TPX_RSP_ID = rspq_overlay_register(&rsp_tinypx);
  tpx_matrix_set(&matrixStack[0], false);
}

inline static void tpx_dmem_set_u32(uint32_t addr, uint32_t value) {
  addr &= 0xFFFF;
  rspq_write(TPX_RSP_ID, TPX_CMD_SET_DMEM, addr, value);
}

inline static void tpx_dmem_set_u16(uint32_t addr, uint32_t value) {
  addr &= 0xFFFF;
  rspq_write(TPX_RSP_ID, TPX_CMD_SET_DMEM, addr | 0x8000, value);
}

void tpx_state_from_t3d()
{
  T3DViewport *vp = t3d_viewport_get();
  assertf(vp, "No Viewport attached");
  uint16_t normWScale = (uint16_t)roundf(0xFFFF * vp->_normScaleW);

  uint32_t addrMatrix = (uint32_t)rsp_tiny3d.data + (RSP_T3D_MATRIX_PROJ & 0xFFFF);
  uint32_t addrScreen = (uint32_t)rsp_tiny3d.data + (RSP_T3D_SCREEN_SCALE_OFFSET & 0xFFFF);
  rspq_write(TPX_RSP_ID, TPX_CMD_SYNC_T3D,
    addrMatrix & 0x00FFFFFF,
    addrScreen & 0x00FFFFFF,
    normWScale
  );
}

void tpx_state_set_scale(float scaleX, float scaleY) {
  uint16_t scaleXNorm = (uint16_t)roundf(scaleX * 0x7FFF);
  uint16_t scaleYNorm = (uint16_t)roundf(scaleY * 0x7FFF);
  tpx_dmem_set_u32(RSP_TPX_PARTICLE_SCALE, (scaleXNorm << 16) | scaleYNorm);
}

void tpx_particle_draw(TPXParticle *particles, uint32_t count)
{
  assert((count & 1) == 0);

  for(uint32_t i = 0; i < count; i += MAX_PARTICLES_COLOR) {
    uint32_t batchSize = (count - i);
    if(batchSize > MAX_PARTICLES_COLOR)batchSize = MAX_PARTICLES_COLOR;

    uint32_t loadSize = sizeof(TPXParticle) * batchSize / 2;
    rdpq_write(-1, TPX_RSP_ID, TPX_CMD_DRAW_COLOR,
        loadSize, (uint32_t)UncachedAddr(particles)
    );

    particles += MAX_PARTICLES_COLOR / 2;
  }
}

inline static void tpx_matrix_stack(void *mat, int32_t stackAdvance, bool doMultiply, bool onlyStackMove) {
  uint32_t advanceMask = (uint32_t)(stackAdvance << 8) & 0x00FFFF00;
  rspq_write(TPX_RSP_ID, TPX_CMD_MATRIX_STACK,
    advanceMask | (onlyStackMove ? 2 : 0) | (doMultiply ? 1 : 0),
    PhysicalAddr(mat)
  );
}

void tpx_matrix_set(const T3DMat4FP *mat, bool doMultiply) {
  tpx_matrix_stack((void*)mat, 0, doMultiply, false);
}

void tpx_matrix_push(const T3DMat4FP *mat) {
  tpx_matrix_stack((void*)mat, sizeof(T3DMat4FP), true, false);
}

void tpx_matrix_pop(int count) {
  int32_t stackAdvance = -((int)sizeof(T3DMat4FP) * count);
  tpx_matrix_stack(NULL, stackAdvance, false, false);
}

void tpx_matrix_push_pos(int count) {
  int32_t stackAdvance = sizeof(T3DMat4FP) * count;
  tpx_matrix_stack(NULL, stackAdvance, false, true);
}

void tpx_buffer_swap(TPXParticle pt[], uint32_t idxA, uint32_t idxB) {
  uint32_t *dataA = (uint32_t*)&pt[idxA/2];
  uint32_t *dataB = (uint32_t*)&pt[idxB/2];

  dataA += idxA & 1;
  dataB += idxB & 1;

  SWAP_U32(dataA[0], dataB[0]);
  SWAP_U32(dataA[2], dataB[2]);
}

void tpx_buffer_copy(TPXParticle *pt, uint32_t idxDst, uint32_t idxSrc) {
  uint32_t *dataDst = (uint32_t*)&pt[idxDst/2];
  uint32_t *dataSrc = (uint32_t*)&pt[idxSrc/2];

  dataDst += idxDst & 1;
  dataSrc += idxSrc & 1;

  dataDst[0] = dataSrc[0];
  dataDst[2] = dataSrc[2];
}

void tpx_destroy()
{
  if(matrixStack)
  {
    free_uncached(matrixStack);
    matrixStack = NULL;
  }
  rspq_overlay_unregister(TPX_RSP_ID);
  TPX_RSP_ID = 0;
}
