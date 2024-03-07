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

#define MODEL_MATRIX_SIZE 0x40

// Screen-Data
static surface_t zbuffer;
static float aspectRatio;

// Camera Data
static T3DMat4 matCamera;
static T3DMat4FP matCameraFP;

// Projection
static T3DMat4 matProj;
static T3DMat4FP matProjFP;

__attribute__((unused))
static int16_t float_to_s16(float val)
{
  float valNorm = val * 32768.f;
  if(valNorm >= 32768.f)return 0x7FFF;
  if(valNorm < -32768.f)return 0x8000;
  return (int16_t)floor(valNorm);
}

void t3d_init(void)
{
  rspq_init();
  void* state = UncachedAddr(rspq_overlay_get_state(&rsp_tiny3d));
  memset(state, 0, 0x400);
  T3D_RSP_ID = rspq_overlay_register(&rsp_tiny3d);

  zbuffer = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

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

void t3d_screen_set_size(uint32_t width, uint32_t height, int guardBandScale, int isReject)
{
  //int16_t sheight = (int)0 - (int16_t)height;
  int16_t sheight = (int16_t)height;
  aspectRatio = (float)width / (float)height;
  uint32_t screenSize2x = (width << 17) | (height<<1);
  uint32_t screenSize4x = (width << 18) | ((uint16_t)(sheight<<2) & 0xFFFF);

  guardBandScale &= 0xF;
  rspq_write(T3D_RSP_ID, T3D_CMD_SCREEN_SIZE,
    guardBandScale | (isReject << 16), screenSize2x, screenSize4x
    //,wFactor
  );
}

void t3d_mat_set(T3DMat4FP *mat, uint32_t idxDst) {
  t3d_matrix_set_mul(mat, idxDst, idxDst);
}

void t3d_matrix_set_mul(T3DMat4FP *mat, uint32_t idxDst, uint32_t idxMul) {
  idxDst *= MODEL_MATRIX_SIZE;
  idxMul *= MODEL_MATRIX_SIZE;

  rspq_write(T3D_RSP_ID, T3D_CMD_MAT_SET,
    PhysicalAddr(mat),
    idxDst << 16 | idxMul
  );
}

void t3d_matrix_push(T3DMat4FP *mat, bool multiply) {
  //dump_backtrace();
  assertf(0, "t3d_matrix_push() Not implemented!");
}

void t3d_mat_proj_set(T3DMat4FP *mat) {
  rspq_write(T3D_RSP_ID, T3D_CMD_PROJ_SET, PhysicalAddr(mat));
}

void t3d_mat_read(void *mat) {
  rspq_write(T3D_RSP_ID, T3D_CMD_MAT_READ,
    0,
    PhysicalAddr(mat)
  );
}

void t3d_vert_load(const T3DVertPacked *vertices, uint32_t offsetSrc, uint32_t count) {
  uint8_t offsetDst = 0;
  count &= ~1; // always load in pairs of 2
  count *= 0x10;

  rspq_write(T3D_RSP_ID, T3D_CMD_VERT_LOAD,
    (offsetDst << 16) | count,
    PhysicalAddr(vertices) + (offsetSrc * 0x10)
  );
}

void t3d_frame_start(void) {
  rdpq_attach(display_get(), &zbuffer);

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

void t3d_camera_look_at(const T3DVec3 *eye, const T3DVec3 *target) {
  t3d_mat4_look_at(&matCamera, eye, target);
  t3d_mat4_to_fixed(&matCameraFP, &matCamera);

  data_cache_hit_writeback_invalidate(&matCameraFP, sizeof(matCameraFP));
  t3d_mat_set(&matCameraFP, 0);

  T3DVec3 camDir;
  t3d_vec3_diff(&camDir, target, eye);
  t3d_vec3_norm(&camDir);
  t3d_set_camera(eye, &camDir);
}

const T3DMat4 *t3d_camera_get_matrix() {
  return &matCamera;
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
  t3d_mat3_mul_vec3(&lightDirView, &matCamera, dir);
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

void t3d_projection_perspective(float fov, float near, float far) {
  t3d_mat4_perspective(&matProj, fov, aspectRatio, near, far);
  t3d_mat4_to_fixed(&matProjFP, &matProj);

  data_cache_hit_writeback_invalidate(&matProjFP, sizeof(matProjFP));
  t3d_mat_proj_set(&matProjFP);
}

const T3DMat4 *t3d_projection_get_matrix() {
  return &matProj;
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
  v0 *= 36; // @TODO: share const with RSPL
  v1 *= 36;
  v2 *= 36;

  v0 += RSP_T3D_TRI_BUFFER & 0xFFFF;
  v1 += RSP_T3D_TRI_BUFFER & 0xFFFF;
  v2 += RSP_T3D_TRI_BUFFER & 0xFFFF;

  uint32_t v12 = (v1 << 16) | v2;
  rdpq_write(-1, T3D_RSP_ID, T3D_CMD_TRI_DRAW,
    v0, v12
  );
}

void t3d_fog_set_range(float near, float far) {
  if(near == 0.0f) {
    rspq_write(T3D_RSP_ID, T3D_CMD_FOG_RANGE, 0, 0);
    return;
  }

  //float scale = 1.0f / (far - near);
  //float offset = near;// * scale;
  float scale = near * 2.0f * 100.0f;
  float offset = -far * 2.0f;

  scale  = fm_floorf(scale   * 1.0f);
  scale  = CLAMP(scale,  -32768.0f, 32767.0f);

  uint16_t fogMul16    = (int16_t)scale & 0xFFFF;
  int32_t fogOffset32 = T3D_F32_TO_FIXED(offset);

  rspq_write(T3D_RSP_ID, T3D_CMD_FOG_RANGE,
    fogMul16, fogOffset32
  );
}
