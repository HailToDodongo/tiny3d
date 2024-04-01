/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#include <t3d/t3d.h>
#include "rsp/rsp_tiny3d.h"

DEFINE_RSP_UCODE(rsp_tiny3d);
uint32_t T3D_RSP_ID = 0;

#define MAX(a,b) (a) > (b) ? (a) : (b)
#define MIN(a,b) (a) < (b) ? (a) : (b)
#define CLAMP(x, min, max) MIN(MAX((x), (min)), (max))

#define VERT_INPUT_SIZE  16
#define VERT_OUTPUT_SIZE 36

static T3DViewport *currentViewport = NULL;
static T3DMat4FP *matrixStack = NULL;

void t3d_init(T3DInitParams params)
{
  if(params.matrixStackSize <= 0)params.matrixStackSize = 8;

  rspq_init();
  char* state = (char*)UncachedAddr(rspq_overlay_get_state(&rsp_tiny3d));

  // Allocate matrix stack and let the ucode know where it is
  matrixStack = malloc_uncached(sizeof(T3DMat4FP) * params.matrixStackSize);

  uint32_t *stackPtr = (uint32_t*)((char*)state + ((RSP_T3D_MATRIX_STACK_PTR - RSP_T3D_STATE_MEM_START) & 0xFFFF));
  *stackPtr = (uint32_t)UncachedAddr(matrixStack);

  T3D_RSP_ID = rspq_overlay_register(&rsp_tiny3d);

  // It's very common to run into under-flows, to avoid costly checks
  // and keep the same behavior as other platforms (e.g. x86) disable it
  uint32_t fcr31 = C1_FCR31();
  fcr31 &= ~(C1_ENABLE_UNDERFLOW);
  C1_WRITE_FCR31(fcr31);
}

void t3d_destroy(void)
{
  rspq_overlay_unregister(T3D_RSP_ID);
  T3D_RSP_ID = 0;
}

void t3d_screen_clear_color(color_t color) {
  rdpq_clear(color);
}

void t3d_screen_clear_depth() {
  rdpq_clear_z(0xFFFC);
}

inline static void t3d_matrix_stack(void *mat, int32_t stackAdvance, bool doMultiply, bool onlyStackMove) {
  rspq_write(T3D_RSP_ID, T3D_CMD_MATRIX_STACK,
    PhysicalAddr(mat), (stackAdvance << 16) | (onlyStackMove ? 2 : 0) | (doMultiply ? 1 : 0)
  );
}

void t3d_matrix_set(const T3DMat4FP *mat, bool doMultiply) {
  t3d_matrix_stack((void*)mat, 0, doMultiply, false);
}

void t3d_matrix_push(const T3DMat4FP *mat) {
  t3d_matrix_stack((void*)mat, sizeof(T3DMat4FP), true, false);
}

void t3d_matrix_pop(int count) {
  int32_t stackAdvance = -((int)sizeof(T3DMat4FP) * count);
  t3d_matrix_stack(NULL, stackAdvance, false, false);
}

void t3d_matrix_push_pos(int count) {
  int32_t stackAdvance = sizeof(T3DMat4FP) * count;
  t3d_matrix_stack(NULL, stackAdvance, false, true);
}

void t3d_matrix_set_proj(const T3DMat4FP *mat) {
  rspq_write(T3D_RSP_ID, T3D_CMD_PROJ_SET, PhysicalAddr(mat));
}

/*void t3d_debug_read(void *mat) {
  rspq_write(T3D_RSP_ID, T3D_CMD_DEBUG_READ,
    0,
    PhysicalAddr(mat)
  );
}*/

void t3d_vert_load(const T3DVertPacked *vertices, uint32_t offset, uint32_t count) {
  uint32_t inputSize = (count & ~1) * VERT_INPUT_SIZE; // always load in pairs of 2

  // calculate where to start the DMA, this may overlap the buffer of transformed vertices
  // we have to place it so that racing the input data is possible
  uint32_t tmpBufferEnd = (RSP_T3D_CLIP_BUFFER_RESULT & 0xFFFF) + 6*16;
  uint16_t offsetDest = tmpBufferEnd - inputSize;
  offsetDest = (offsetDest & ~0xF); // make sure it's aligned to 16 bytes, must be aligned backwards

  // DMEM address where the transformed vertices are stored
  // must be within RSP_T3D_TRI_BUFFER, alignment is not required
  uint16_t offsetInput = RSP_T3D_TRI_BUFFER & 0xFFFF;
  offsetInput += offset * VERT_OUTPUT_SIZE;

  rspq_write(T3D_RSP_ID, T3D_CMD_VERT_LOAD,
    inputSize,
    PhysicalAddr(vertices),
    (offsetDest << 16) | offsetInput
  );
}

void t3d_frame_start(void) {
  // Reset render state
  rdpq_mode_begin();
    rdpq_set_mode_standard();
    rdpq_mode_antialias(AA_STANDARD);
    rdpq_mode_zbuf(true, true);
    rdpq_mode_persp(true);

    rdpq_mode_filter(FILTER_BILINEAR);
    rdpq_mode_dithering(DITHER_SQUARE_SQUARE);

    //rdpq_mode_blender(0);
    rdpq_mode_fog(0);
  rdpq_mode_end();
}

void t3d_light_set_ambient(const uint8_t *color)
{
  rspq_write(T3D_RSP_ID, T3D_CMD_LIGHT_SET,
    (RSP_T3D_COLOR_AMBIENT & 0xFFFF), // address
    (color[0] << 24) | (color[1] << 16) | (color[2] << 8) | color[3],
    0 // dir
  );
}

void t3d_light_set_directional(int index, const uint8_t *color, const T3DVec3 *dir)
{
  T3DVec3 lightDirView;
  if(currentViewport) {
    t3d_mat3_mul_vec3(&lightDirView, &currentViewport->matCamera, dir);
  } else {
  lightDirView = *dir;
  }
  t3d_vec3_norm(&lightDirView);

  uint8_t dirFP[3] = {
    (uint8_t)(int8_t)(lightDirView.v[0] * 127.0f),
    (uint8_t)(int8_t)(lightDirView.v[1] * 127.0f),
    (uint8_t)(int8_t)(lightDirView.v[2] * 127.0f),
  };

  index = (index*16) + RSP_T3D_LIGHT_DIR_COLOR;
  index &= 0xFFFF;

  rspq_write(T3D_RSP_ID, T3D_CMD_LIGHT_SET,
    index, // address
    (color[0] << 24) | (color[1] << 16) | (color[2] << 8) | color[3],
    (dirFP[0] << 24) | (dirFP[1] << 16) | (dirFP[2] << 8)
  );
}

uint16_t t3d_vert_pack_normal(const T3DVec3 *normal)
{
  T3DVec3 norm = *normal;
  t3d_vec3_norm(&norm);

  int32_t xInt = (int32_t)roundf(norm.v[0] * 15.5f);
  int32_t yInt = (int32_t)roundf(norm.v[1] * 15.5f);
  int32_t zInt = (int32_t)roundf(norm.v[2] * 15.5f);
  xInt = CLAMP(xInt, -16, 15);
  yInt = CLAMP(yInt, -16, 15);
  zInt = CLAMP(zInt, -16, 15);

  return ((uint16_t)(xInt) & 0b11111) << 10
       | ((uint16_t)(yInt) & 0b11111) <<  5
       | ((uint16_t)(zInt) & 0b11111) <<  0;
}

void t3d_state_set_drawflags(enum T3DDrawFlags drawFlags)
{
  bool cullFront = drawFlags & T3D_FLAG_CULL_FRONT;
  bool cullBack = drawFlags & T3D_FLAG_CULL_BACK;

  // filter out T3D specific flags since RSPQ doesn't support them
  drawFlags &= ~(T3D_FLAG_CULL_FRONT | T3D_FLAG_CULL_BACK);

  assert(!(cullFront && cullBack));

  uint32_t cullMask;
  if(!cullFront && !cullBack) {
    cullMask = 2;
  } else if(cullFront) {
    cullMask = 1;
  } else {
    cullMask = 0;
  }

  uint32_t cmd = drawFlags | RDPQ_CMD_TRI;
  cmd = 0xC000 | (cmd << 8);
  rspq_write(T3D_RSP_ID, T3D_CMD_DRAWFLAGS, cullMask, cmd);
}

void t3d_tri_draw(uint32_t v0, uint32_t v1, uint32_t v2)
{
  v0 *= VERT_OUTPUT_SIZE;
  v1 *= VERT_OUTPUT_SIZE;
  v2 *= VERT_OUTPUT_SIZE;

  v0 += RSP_T3D_TRI_BUFFER & 0xFFFF;
  v1 += RSP_T3D_TRI_BUFFER & 0xFFFF;
  v2 += RSP_T3D_TRI_BUFFER & 0xFFFF;

  uint32_t v12 = (v1 << 16) | v2;
  rdpq_write(-1, T3D_RSP_ID, T3D_CMD_TRI_DRAW,
    v0, v12
  );
}

void t3d_fog_set_range(float near, float far) {
  if(near == 0.0f && far == 0.0f) {
    rspq_write(T3D_RSP_ID, T3D_CMD_FOG_RANGE, 0, 0);
    return;
  }

  // prevent diff by zero and weird values
  float diff = far - near;
  if(fabsf(diff) < 1.5f) {
    diff = 1.5f;
  }

  // @TODO: refactor in the ucode (right now it's offset and then scale)
  float scale = 16384.0f / diff;
  float offset = -near * 2.0f;

  scale = fm_floorf(scale);
  scale = CLAMP(scale,  -32768.0f, 32767.0f);

  uint16_t fogMul16   = (int16_t)scale & 0xFFFF;
  int32_t fogOffset32 = T3D_F32_TO_FIXED(offset);

  rspq_write(T3D_RSP_ID, T3D_CMD_FOG_RANGE,
    fogMul16, fogOffset32
  );
}

void t3d_viewport_attach(T3DViewport *viewport) {
  assertf(viewport != NULL, "Viewport is NULL!");
  currentViewport = viewport;

  // Limit draw region
  rdpq_set_scissor(
    viewport->offset[0], viewport->offset[1],
    viewport->offset[0] + viewport->size[0],
    viewport->offset[1] + viewport->size[1]
  );

  // Set screen size, internally the 3D-scene renders to the correct size, but at [0,0]
  // calc. both scale and offset to move/scale it into our scissor region
  int32_t screenOffsetX = (int32_t)(viewport->offset[0]*2) + viewport->size[0];
  int32_t screenOffsetY = (int32_t)(viewport->offset[1]*2) + viewport->size[1];

  int32_t screenScaleY = -viewport->size[1];

  int32_t screenOffset = (screenOffsetX << 17) | (screenOffsetY << 1);
  int32_t screenScale = (viewport->size[0] << 18)
    | ((uint16_t)(screenScaleY<<2) & 0xFFFF);

  int32_t guardBandScale = viewport->guardBandScale & 0xF;
  rspq_write(T3D_RSP_ID, T3D_CMD_SCREEN_SIZE,
    guardBandScale | (viewport->useRejection << 16),
    screenOffset, screenScale
  );

  // update projection matrix
  t3d_mat4_to_fixed(&viewport->_matProjFP, &viewport->matProj);
  data_cache_hit_writeback(&viewport->_matProjFP, sizeof(T3DMat4FP));
  t3d_matrix_set_proj(&viewport->_matProjFP);

  // update camera matrix
  t3d_mat4_to_fixed(&viewport->_matCameraFP, &viewport->matCamera);
  data_cache_hit_writeback(&viewport->_matCameraFP, sizeof(T3DMat4FP));
  t3d_matrix_set(&viewport->_matCameraFP, false);
}

void t3d_viewport_set_projection(T3DViewport *viewport, float fov, float near, float far) {
  float aspectRatio = (float)viewport->size[0] / (float)viewport->size[1];
  t3d_mat4_perspective(&viewport->matProj, fov, aspectRatio, near, far);
}

void t3d_viewport_look_at(T3DViewport *viewport, const T3DVec3 *eye, const T3DVec3 *target, const T3DVec3 *up) {
  t3d_mat4_look_at(&viewport->matCamera, eye, target, up);
}

