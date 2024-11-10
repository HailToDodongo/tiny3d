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

void tpx_init([[maybe_unused]] TPXInitParams params)
{
  TPX_RSP_ID = rspq_overlay_register(&rsp_tinypx);
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
  count &= ~1;
  assert(count <= 344);
  count = sizeof(TPXParticle) * count / 2;
  rdpq_write(-1, TPX_RSP_ID, TPX_CMD_DRAW_COLOR,
      count, (uint32_t)UncachedAddr(particles)
   );
}

void tpx_destroy()
{
  rspq_overlay_unregister(TPX_RSP_ID);
  TPX_RSP_ID = 0;
}
