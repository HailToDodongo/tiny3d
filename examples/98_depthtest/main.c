#include <libdragon.h>
#include <t3d/t3d.h>
#include <math.h>

/**
 * 98_depthtest - depth precision measurement scenes.
 *
 * Deterministic test scenes for measuring the real depth pipeline
 * (vertex Z, slopes, z-buffer encoding, comparator) under an emulator that
 * exposes the 18-bit z-buffer. Nothing here depends on time or frame rate:
 * the whole state is a handful of integers changed via controller input,
 * and every frame is rendered synchronously (rspq_wait) and re-rendered
 * identically until the state changes.
 *
 * Camera: origin, looking down -Z, vertical FOV 60deg, screen 320x240.
 *
 * Scenes (C-left / C-right):
 *   0 SWEEP  - view-aligned quad at distance w(step), log-spaced between
 *              1.05*near and 0.99*far over SWEEP_STEPS steps. Every pixel
 *              must hold the same code; the sweep gives bits / monotonicity /
 *              vertex error (fit Z = A + B/w on the host).
 *              sub 0: quad covers ~80% of the screen (never clipped)
 *              sub 1: quad 2.5x oversized (goes through the clipper)
 *   1 PLANE  - floor plane y = -h from z=-z0 (near) to z=-z1 (far),
 *              per-pixel ideal depth is analytic on the host.
 *              sub 0: single quad (2 tris), red         -> slope error
 *              sub 1: dense grid PLANE_DIV^2, green     -> vertex error
 *              sub 2: single opaque red + dense DECAL green -> decal fail %
 *              sub 3: single opaque red + dense opaque green (coplanar fight)
 *              step:  selects the plane height h (0..3), see printout
 *   2 SEP    - 4x4 cells; each cell = red back quad at distance d and an
 *              inset green front quad at d - g, g/d log-spaced per cell
 *              (see the printed gap list). A cell passes when the inset is
 *              all green. step = distance index (SEP_DISTANCES, log-spaced),
 *              sub 0: view-aligned, sub 1: both quads tilted 30deg about X.
 *
 * Controls:
 *   D-pad left/right  step -1/+1      D-pad down/up  step -100/+100
 *   C-left/C-right    scene -1/+1     A / B          sub +1/-1
 *   L / R             near/far preset -1/+1
 *   Z                 step = 0        Start          auto-advance step each frame
 *
 * Protocol (ISViewer/usblog): every rendered frame prints one line
 *   "DT scene=<s> sub=<u> step=<k> near=<n> far=<f> <scene params>"
 * after the frame has fully completed. In addition the current
 * scene/sub/step is encoded into the colour buffer as a 24-cell barcode on
 * the bottom row (y >= MARKER_Y, outside the measured area), MSB first,
 * 4x4 px per bit, green = 1 / dark blue = 0: scene(4) sub(4) step(16).
 */

#define SCREEN_W 320
#define SCREEN_H 240
#define FOV_DEG 60.0f
#define SWEEP_STEPS 2000
#define SEP_DISTANCES 8
#define SEP_GRID 4
#define PLANE_DIV 32
#define MARKER_Y 232
#define MARKER_CELL 4

typedef struct { float near, far; } Preset;
static const Preset PRESETS[] = {{10, 12800}, {10, 1000}, {10, 150}, {1, 100}};
#define PRESET_COUNT (sizeof(PRESETS) / sizeof(PRESETS[0]))

enum { SCENE_SWEEP, SCENE_PLANE, SCENE_SEP, SCENE_COUNT };
static const int SUB_COUNT[SCENE_COUNT] = {2, 4, 2};
static const int STEP_COUNT[SCENE_COUNT] = {SWEEP_STEPS, 4, SEP_DISTANCES};

// initial state can be set at compile time (e.g. -DDT_INIT_SCENE=2) for scripted runs
#ifndef DT_INIT_SCENE
#define DT_INIT_SCENE 0
#endif
#ifndef DT_INIT_SUB
#define DT_INIT_SUB 0
#endif
#ifndef DT_INIT_STEP
#define DT_INIT_STEP 0
#endif
#ifndef DT_INIT_PRESET
#define DT_INIT_PRESET 0
#endif

static struct {
  int scene, sub, step, preset;
  bool autoAdvance;
} st = {DT_INIT_SCENE, DT_INIT_SUB, DT_INIT_STEP, DT_INIT_PRESET, false};

static float tanHalfFov, aspect;

// ---- pools (reset per frame; safe because every frame ends in rspq_wait) --
#define VERT_POOL 4096
#define MAT_POOL 64
static T3DVertPacked *vertPool;
static T3DMat4FP *matPool;
static int vertUsed, matUsed;
static uint16_t packedNormal;

static T3DVertPacked *vertAlloc(int count) {
  int packed = (count + 1) / 2;
  assertf(vertUsed + packed <= VERT_POOL / 2, "vertex pool exhausted");
  T3DVertPacked *p = &vertPool[vertUsed];
  vertUsed += packed;
  return p;
}

static T3DMat4FP *matAlloc(void) {
  assertf(matUsed < MAT_POOL, "matrix pool exhausted");
  return &matPool[matUsed++];
}

static void setVert(T3DVertPacked *buf, int i, float x, float y, float z, uint32_t rgba) {
  T3DVertPacked *p = &buf[i >> 1];
  int16_t px = (int16_t)lroundf(x), py = (int16_t)lroundf(y), pz = (int16_t)lroundf(z);
  if (i & 1) {
    p->posB[0] = px; p->posB[1] = py; p->posB[2] = pz;
    p->rgbaB = rgba; p->normB = packedNormal; p->stB[0] = 0; p->stB[1] = 0;
  } else {
    p->posA[0] = px; p->posA[1] = py; p->posA[2] = pz;
    p->rgbaA = rgba; p->normA = packedNormal; p->stA[0] = 0; p->stA[1] = 0;
  }
}

// quad in the XY plane at z=0, centred, half extents hx/hy (under `mat`)
static void drawQuadXY(float hx, float hy, uint32_t color, const T3DMat4FP *mat) {
  T3DVertPacked *v = vertAlloc(4);
  setVert(v, 0, -hx, -hy, 0, color);
  setVert(v, 1,  hx, -hy, 0, color);
  setVert(v, 2,  hx,  hy, 0, color);
  setVert(v, 3, -hx,  hy, 0, color);
  t3d_matrix_push(mat);
  t3d_vert_load(v, 0, 4);
  t3d_tri_draw(0, 1, 2);
  t3d_tri_draw(0, 2, 3);
  t3d_tri_sync();
  t3d_matrix_pop(1);
}

// floor plane y=-h, x in [-hw, hw] (nx segments), z at zs[0..nz] (world coords, identity matrix)
static void drawFloorGrid(float h, float hw, const int32_t *zs, int nz, int nx, uint32_t color) {
  assertf(2 * (nx + 1) <= T3D_VERTEX_CACHE_SIZE, "too many columns");
  T3DMat4FP *ident = matAlloc();
  t3d_mat4fp_identity(ident);
  t3d_matrix_push(ident);
  for (int j = 0; j < nz; ++j) {
    int cols = nx + 1;
    T3DVertPacked *v = vertAlloc(2 * cols);
    for (int i = 0; i < cols; ++i) {
      float x = -hw + 2.0f * hw * (float)i / (float)nx;
      setVert(v, i,        x, -h, (float)zs[j],     color);
      setVert(v, cols + i, x, -h, (float)zs[j + 1], color);
    }
    t3d_vert_load(v, 0, 2 * cols);
    for (int i = 0; i < nx; ++i) {
      t3d_tri_draw(i, i + 1, cols + i);
      t3d_tri_draw(i + 1, cols + i + 1, cols + i);
    }
    t3d_tri_sync();
  }
  t3d_matrix_pop(1);
}

static void setMatSRT(T3DMat4FP *m, float sx, float sy, float sz, float rx, float tx, float ty, float tz) {
  t3d_mat4fp_from_srt_euler(m, (float[3]){sx, sy, sz}, (float[3]){rx, 0, 0}, (float[3]){tx, ty, tz});
}

// ---- scenes ---------------------------------------------------------------

static float sweepDistance(int step, float near, float far) {
  // tiny3d's near clip currently sits at 2fn/(2f-n) (up to ~3.5% beyond near), start past it
  float w0 = near * 1.05f, w1 = far * 0.99f;
  return w0 * powf(w1 / w0, (float)step / (float)(SWEEP_STEPS - 1));
}

static void sceneSweep(float near, float far, char *info, size_t infoLen) {
  float w = sweepDistance(st.step, near, far);
  float cover = st.sub == 0 ? 0.8f : 2.5f;
  float hy = w * tanHalfFov * cover, hx = hy * aspect;
  T3DMat4FP *m = matAlloc();
  setMatSRT(m, hx / 1000.0f, hy / 1000.0f, 1.0f, 0, 0, 0, -w);
  drawQuadXY(1000, 1000, 0xFFFFFFFF, m);
  snprintf(info, infoLen, "w=%.7g cover=%.2f", w, cover);
}

static void scenePlane(float near, float far, char *info, size_t infoLen) {
  // h: fraction of the near-plane half height; the near edge of the plane
  // must stay inside the frustum so nothing gets clipped
  const float hFrac[4] = {0.8f, 0.5f, 0.3f, 0.95f};
  float z0f = near * 1.1f, z1f = far * 0.95f;
  float h = roundf(z0f * tanHalfFov * hFrac[st.step]);
  float hw = roundf(0.9f * z0f * tanHalfFov * aspect);
  static int32_t zs[PLANE_DIV + 1];
  for (int j = 0; j <= PLANE_DIV; ++j)
    zs[j] = -(int32_t)lroundf(z0f * powf(z1f / z0f, (float)j / (float)PLANE_DIV));
  int32_t zSingle[2] = {zs[0], zs[PLANE_DIV]};

  const uint32_t RED = 0xFF0000FF, GREEN = 0x00FF00FF;
  switch (st.sub) {
    case 0: drawFloorGrid(h, hw, zSingle, 1, 1, RED); break;
    case 1: drawFloorGrid(h, hw, zs, PLANE_DIV, PLANE_DIV, GREEN); break;
    case 2:
      drawFloorGrid(h, hw, zSingle, 1, 1, RED);
      rdpq_change_other_modes_raw(SOM_ZMODE_MASK, SOM_ZMODE_DECAL);
      drawFloorGrid(h, hw, zs, PLANE_DIV, PLANE_DIV, GREEN);
      rdpq_change_other_modes_raw(SOM_ZMODE_MASK, 0);
      break;
    case 3:
      drawFloorGrid(h, hw, zSingle, 1, 1, RED);
      drawFloorGrid(h, hw, zs, PLANE_DIV, PLANE_DIV, GREEN);
      break;
  }
  int n = snprintf(info, infoLen, "h=%g hw=%g div=%d zs=", h, hw, PLANE_DIV);
  for (int j = 0; j <= PLANE_DIV && n < (int)infoLen; ++j)
    n += snprintf(info + n, infoLen - n, "%ld%s", zs[j], j < PLANE_DIV ? "," : "");
}

static float sepGapFrac(int cell) {
  // g/d from 1/8 down to ~1/19500, log-spaced over the 16 cells
  return powf(2.0f, -(3.0f + 0.75f * (float)cell));
}

static void sceneSep(float near, float far, char *info, size_t infoLen) {
  float d0 = near * 1.2f, d1 = far * 0.9f;
  float d = d0 * powf(d1 / d0, (float)st.step / (float)(SEP_DISTANCES - 1));
  float tilt = st.sub == 1 ? T3D_DEG_TO_RAD(30.0f) : 0.0f;
  const uint32_t RED = 0xFF0000FF, GREEN = 0x00FF00FF;

  for (int cy = 0; cy < SEP_GRID; ++cy) {
    for (int cx = 0; cx < SEP_GRID; ++cx) {
      int cell = cy * SEP_GRID + cx;
      float g = d * sepGapFrac(cell);
      // cell centre in NDC (y up), half size of a cell in NDC
      float ncx = -1.0f + (2.0f * cx + 1.0f) / SEP_GRID;
      float ncy =  1.0f - (2.0f * cy + 1.0f) / SEP_GRID;
      float nh = 1.0f / SEP_GRID;

      // back quad: cell shrunk to 90%, at distance d
      float db = d;
      T3DMat4FP *mb = matAlloc();
      setMatSRT(mb, 1, 1, 1, tilt, ncx * db * tanHalfFov * aspect, ncy * db * tanHalfFov, -db);
      drawQuadXY(nh * 0.9f * db * tanHalfFov * aspect, nh * 0.9f * db * tanHalfFov, RED, mb);

      // front quad: cell shrunk to 40%, at distance d - g
      float df = d - g;
      T3DMat4FP *mf = matAlloc();
      setMatSRT(mf, 1, 1, 1, tilt, ncx * df * tanHalfFov * aspect, ncy * df * tanHalfFov, -df);
      drawQuadXY(nh * 0.4f * df * tanHalfFov * aspect, nh * 0.4f * df * tanHalfFov, GREEN, mf);
    }
  }
  int n = snprintf(info, infoLen, "d=%.7g tiltDeg=%d grid=%d gapFrac=", d, st.sub == 1 ? 30 : 0, SEP_GRID);
  for (int c = 0; c < SEP_GRID * SEP_GRID && n < (int)infoLen; ++c)
    n += snprintf(info + n, infoLen - n, "%.6g%s", sepGapFrac(c), c < SEP_GRID * SEP_GRID - 1 ? "," : "");
}

// ---- state marker (colour buffer only, bottom row) --------------------------
static void drawMarker(void) {
  uint32_t bits = ((uint32_t)st.scene << 20) | ((uint32_t)st.sub << 16) | ((uint32_t)st.step & 0xFFFF);
  for (int b = 0; b < 24; ++b) {
    bool one = (bits >> (23 - b)) & 1;
    rdpq_set_mode_fill(one ? RGBA32(0, 255, 0, 255) : RGBA32(0, 0, 128, 255));
    int x = b * MARKER_CELL;
    rdpq_fill_rectangle(x, MARKER_Y, x + MARKER_CELL, MARKER_Y + MARKER_CELL);
  }
}

// ---- input -----------------------------------------------------------------
static void clampState(void) {
  if (st.scene < 0) st.scene = SCENE_COUNT - 1;
  if (st.scene >= SCENE_COUNT) st.scene = 0;
  if (st.sub < 0) st.sub = SUB_COUNT[st.scene] - 1;
  if (st.sub >= SUB_COUNT[st.scene]) st.sub = 0;
  if (st.step < 0) st.step = 0;
  if (st.step >= STEP_COUNT[st.scene]) st.step = STEP_COUNT[st.scene] - 1;
  if (st.preset < 0) st.preset = PRESET_COUNT - 1;
  if (st.preset >= (int)PRESET_COUNT) st.preset = 0;
}

static void handleInput(void) {
  joypad_poll();
  joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
  if (btn.d_right) st.step += 1;
  if (btn.d_left)  st.step -= 1;
  if (btn.d_up)    st.step += 100;
  if (btn.d_down)  st.step -= 100;
  if (btn.c_right) { st.scene += 1; st.sub = 0; st.step = 0; }
  if (btn.c_left)  { st.scene -= 1; st.sub = 0; st.step = 0; }
  if (btn.a) st.sub += 1;
  if (btn.b) st.sub -= 1;
  if (btn.r) st.preset += 1;
  if (btn.l) st.preset -= 1;
  if (btn.z) st.step = 0;
  if (btn.start) st.autoAdvance = !st.autoAdvance;
  if (st.autoAdvance) {
    if (st.step + 1 >= STEP_COUNT[st.scene]) st.autoAdvance = false;
    else st.step += 1;
  }
  clampState();
}

int main(void) {
  debug_init_isviewer();
  debug_init_usblog();

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_DISABLED);
  rdpq_init();
  joypad_init();
  t3d_init((T3DInitParams){});

  vertPool = malloc_uncached(sizeof(T3DVertPacked) * (VERT_POOL / 2));
  matPool = malloc_uncached(sizeof(T3DMat4FP) * MAT_POOL);
  packedNormal = t3d_vert_pack_normal(&(fm_vec3_t){{0, 0, 1}});

  T3DViewport viewport = t3d_viewport_create();
  tanHalfFov = tanf(T3D_DEG_TO_RAD(FOV_DEG) * 0.5f);
  aspect = (float)SCREEN_W / (float)SCREEN_H;

  const fm_vec3_t camPos = {{0, 0, 0}};
  const fm_vec3_t camTarget = {{0, 0, -1}};
  uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0xFF};

  static char info[1024];
  debugf("DT ready screen=%dx%d fovDeg=%g aspect=%.7g sweepSteps=%d presets=%d\n",
         SCREEN_W, SCREEN_H, (double)FOV_DEG, (double)aspect, SWEEP_STEPS, (int)PRESET_COUNT);

  for (;;) {
    handleInput();
    const Preset *p = &PRESETS[st.preset];

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(FOV_DEG), p->near, p->far);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(fm_vec3_t){{0, 1, 0}});

    vertUsed = 0;
    matUsed = 0;
    info[0] = 0;

    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    rdpq_mode_combiner(RDPQ_COMBINER_SHADE);
    t3d_screen_clear_color(RGBA32(0, 0, 0, 0xFF));
    t3d_screen_clear_depth();
    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(0);
    t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_DEPTH);

    switch (st.scene) {
      case SCENE_SWEEP: sceneSweep(p->near, p->far, info, sizeof(info)); break;
      case SCENE_PLANE: scenePlane(p->near, p->far, info, sizeof(info)); break;
      case SCENE_SEP:   sceneSep(p->near, p->far, info, sizeof(info)); break;
    }

    drawMarker();
    // report while still attached: the frame is complete (rspq_wait) and the
    // RDP's colour/depth image registers still point at this frame's buffers,
    // so a host reading them on this line sees exactly the finished frame
    rspq_wait();
    debugf("DT scene=%d sub=%d step=%d near=%g far=%g %s\n",
           st.scene, st.sub, st.step, (double)p->near, (double)p->far, info);
    rdpq_detach_show();
  }
}
