#include <libdragon.h>
#include <t3d/t3d.h>

/**
 * Clipping benchmark / test scene (97_cliptest).
 *
 * Draws a fixed set of hand-made triangles that force every clipping case
 * (one side plane, two side planes, all four, vertices behind the camera),
 * plus a few reference triangles that are not clipped or are rejected.
 * All vertex attributes are used: depth, shaded colour, UVs, fog.
 * Nothing is animated, every frame is identical; 'cliptest.js' profiles the
 * RSP commands and compares a screenshot.
 *
 * Debugging: 'make CT_FLAGS=-DONLY_TRI=n' builds a ROM that draws only triangle n,
 * 'make CT_FLAGS=-DONLY_MASK=0x..' one that draws only the triangles whose bit is set.
 */

typedef struct {
  int16_t pos[3];
  uint32_t rgba;
  int16_t st[2]; // texture pixels
} Vert;

// Camera at z=-40 looking at the origin, FOV 85 deg (4:3):
// at z=0 the screen spans about +-49 x / +-37 y, the guard band (2x) +-98 / +-73.
// A vertex at |x| or |y| >= 200 is always outside the guard band, z <= -50 is behind the camera.
#define RED    0xFF0000FF
#define GREEN  0x00FF00FF
#define BLUE   0x0000FFFF
#define YELLOW 0xFFFF00FF
#define CYAN   0x00FFFFFF
#define PINK   0xFF00FFFF
#define WHITE  0xFFFFFFFF

static const Vert VERTS[] = {
  // 0: +X plane only
  {{  10, -10,   0}, RED,    {0,   0}}, {{ 200, -20,   5}, GREEN,  {96,  0}}, {{  10,  20,  -5}, BLUE,   {0,  64}},
  // 3: -X plane only
  {{ -10, -15,   0}, YELLOW, {0,   0}}, {{-200,  -5,  10}, CYAN,   {96,  0}}, {{ -10,  15,  -8}, PINK,   {0,  64}},
  // 6: +Y plane only
  {{ -20,  10,   0}, GREEN,  {0,   0}}, {{  20,   5,  10}, BLUE,   {32,  0}}, {{   0, 200,   0}, RED,    {0,  96}},
  // 9: -Y plane only
  {{ -20, -10,   0}, CYAN,   {0,   0}}, {{  20,  -5,  -5}, PINK,   {32,  0}}, {{   5,-200,  20}, YELLOW, {0,  96}},
  // 12: +X and +Y (corner)
  {{   0,   0,   0}, WHITE,  {0,   0}}, {{ 200,  30,   0}, RED,    {64,  0}}, {{  30, 200,   0}, BLUE,   {0,  64}},
  // 15: -X and -Y (corner)
  {{   0,   0,   5}, WHITE,  {0,   0}}, {{-200, -30,   0}, GREEN,  {64,  0}}, {{ -30,-200,  10}, YELLOW, {0,  64}},
  // 18: huge background triangle, crosses all four side planes
  {{-300,-300,  15}, RED,    {0,   0}}, {{ 300,-300,  15}, GREEN,  {128, 0}}, {{   0, 300,  15}, BLUE,   {64,128}},
  // 21: one vertex behind the camera
  {{ -15,  -5, -10}, CYAN,   {0,   0}}, {{  15,  -5, -10}, PINK,   {32,  0}}, {{   0, -30, -60}, WHITE,  {16, 32}},
  // 24: two vertices behind the camera
  {{   0,  10, -10}, YELLOW, {16,  0}}, {{ -40,  20, -70}, RED,    {0,  32}}, {{  40,  20, -70}, GREEN,  {32, 32}},
  // 27: tilted floor quad, far edge in front, near edge behind the camera (2 tris)
  {{ -60, -25,  60}, BLUE,   {0,   0}}, {{  60, -25,  60}, CYAN,   {64,  0}}, {{  60, -25, -60}, PINK,   {64, 64}}, {{ -60, -25, -60}, WHITE, {0, 64}},
  // 31: reference quad, fully inside (2 tris, never clipped)
  {{  -8,  -8,  -5}, RED,    {0,   0}}, {{   8,  -8,  -5}, GREEN,  {32,  0}}, {{   8,   8,  -5}, BLUE,   {32, 32}}, {{  -8,   8,  -5}, WHITE, {0, 32}},
  // 35: fully outside, rejected
  {{ 200, 200,   0}, RED,    {0,   0}}, {{ 250, 200,   0}, GREEN,  {32,  0}}, {{ 200, 250,   0}, BLUE,   {0,  32}},
  // 38: crosses the screen edge but stays inside the guard band (scissor only, no clipping)
  {{  30,   0,   0}, YELLOW, {0,   0}}, {{  80, -20,   0}, CYAN,   {32,  0}}, {{  80,  20,   0}, PINK,   {32, 32}},
};
#define VERT_COUNT (sizeof(VERTS) / sizeof(VERTS[0]))

static const uint8_t TRIS[][3] = {
  {0,1,2}, {3,4,5}, {6,7,8}, {9,10,11}, {12,13,14}, {15,16,17}, {18,19,20},
  {21,22,23}, {24,25,26},
  {27,28,29}, {27,29,30},
  {31,32,33}, {31,33,34},
  {35,36,37},
  {38,39,40},
};
#define TRI_COUNT (sizeof(TRIS) / sizeof(TRIS[0]))

int main()
{
  debug_init_isviewer();
  debug_init_usblog();

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
  rdpq_init();
  t3d_init((T3DInitParams){});

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
  t3d_mat4fp_identity(modelMatFP);

  // procedural 32x32 checker texture, no filesystem needed
  surface_t tex = surface_alloc(FMT_RGBA16, 32, 32);
  for(int y=0; y<32; ++y) {
    uint16_t *row = (uint16_t*)((uint8_t*)tex.buffer + y * tex.stride);
    for(int x=0; x<32; ++x) {
      bool check = ((x >> 3) ^ (y >> 3)) & 1;
      uint32_t r = check ? 31 : 8 + (x >> 1);
      uint32_t g = check ? 31 : 8 + (y >> 1);
      uint32_t b = check ? 31 : 20;
      row[x] = (r << 11) | (g << 6) | (b << 1) | 1;
    }
  }
  data_cache_hit_writeback(tex.buffer, tex.stride * 32);

  // pack vertices (UVs are 10.5 fixed point pixel coordinates)
  uint32_t packedCount = (VERT_COUNT + 1) / 2;
  T3DVertPacked* vertices = malloc_uncached(sizeof(T3DVertPacked) * packedCount);
  uint16_t norm = t3d_vert_pack_normal(&(T3DVec3){{0, 0, 1}});
  for(uint32_t i=0; i<VERT_COUNT; ++i) {
    const Vert *v = &VERTS[i];
    T3DVertPacked *p = &vertices[i / 2];
    if(i & 1) {
      p->posB[0] = v->pos[0]; p->posB[1] = v->pos[1]; p->posB[2] = v->pos[2];
      p->rgbaB = v->rgba; p->normB = norm;
      p->stB[0] = v->st[0] << 5; p->stB[1] = v->st[1] << 5;
    } else {
      p->posA[0] = v->pos[0]; p->posA[1] = v->pos[1]; p->posA[2] = v->pos[2];
      p->rgbaA = v->rgba; p->normA = norm;
      p->stA[0] = v->st[0] << 5; p->stA[1] = v->st[1] << 5;
    }
  }

  const T3DVec3 camPos    = {{0, 0, -40}};
  const T3DVec3 camTarget = {{0, 0, 0}};
  uint8_t colorAmbient[4] = {80, 80, 80, 0xFF};
  uint8_t colorDir[4]     = {0xFF, 0xFF, 0xFF, 0xFF};
  T3DVec3 lightDirVec = {{0.0f, 0.0f, 1.0f}};

  T3DViewport viewport = t3d_viewport_create();
  t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 100.0f);
  t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0, 1, 0}});

  rspq_block_t *dplDraw = NULL;

  for(;;)
  {
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(30, 30, 60, 0));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    rdpq_set_fog_color(RGBA32(120, 120, 140, 0xFF));
    t3d_fog_set_range(30.0f, 120.0f);
    t3d_fog_set_enabled(true);

    rdpq_mode_combiner(RDPQ_COMBINER_TEX_SHADE);
    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_tex_upload(TILE0, &tex, &(rdpq_texparms_t){.s.repeats = REPEAT_INFINITE, .t.repeats = REPEAT_INFINITE});

    t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_TEXTURED | T3D_FLAG_DEPTH);

    if(!dplDraw) {
      rspq_block_begin();
      t3d_matrix_push(modelMatFP);
      t3d_vert_load(vertices, 0, packedCount * 2);
      t3d_matrix_pop(1);
      for(uint32_t i=0; i<TRI_COUNT; ++i) {
        #ifdef ONLY_TRI
          if(i != ONLY_TRI)continue;
        #endif
        #ifdef ONLY_MASK
          if(!((ONLY_MASK >> i) & 1))continue;
        #endif
        t3d_tri_draw(TRIS[i][0], TRIS[i][1], TRIS[i][2]);
      }
      t3d_tri_sync();
      dplDraw = rspq_block_end();
    }
    rspq_block_run(dplDraw);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}
