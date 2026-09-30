#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>

/**
 * Animation benchmark (96_animtest).
 *
 * Draws 4 instances of the same model, each with its own skeleton playing a different animation.
 * The CPU time of 't3d_anim_update' and 't3d_skeleton_update' (summed over all instances)
 * is measured every frame and shown as average and peak over the last 'AVG_FRAMES' frames.
 * A fixed delta-time is used so that runs are reproducible.
 */

#define FB_COUNT 3
#define INST_COUNT 4
#define AVG_FRAMES 64
#define DELTA_TIME (1.0f / 60.0f)

typedef struct {
  T3DSkeleton skel;
  T3DAnim anim;
  float posX;
} Instance;

typedef struct {
  uint32_t sum;
  uint32_t peak;
  uint32_t curSum;
  uint32_t curPeak;
} TimeStat;

static void stat_add(TimeStat *s, uint32_t ticks) {
  s->curSum += ticks;
  if(ticks > s->curPeak)s->curPeak = ticks;
}

static void stat_flush(TimeStat *s) {
  s->sum = s->curSum;
  s->peak = s->curPeak;
  s->curSum = 0;
  s->curPeak = 0;
}

int main()
{
  debug_init_isviewer();
  debug_init_usblog();
  asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, FB_COUNT, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  rdpq_init();
  t3d_init((T3DInitParams){});
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  T3DViewport viewport = t3d_viewport_create_buffered(FB_COUNT);
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP) * FB_COUNT * INST_COUNT);

  const T3DVec3 camPos    = {{0, 22.0f, 66.0f}};
  const T3DVec3 camTarget = {{0, 16.0f, 0}};

  uint8_t colorAmbient[4] = {0xBB, 0xBB, 0xBB, 0xFF};
  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};
  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  // "catherine.blend" Model from: https://github.com/buu342/N64-Sausage64
  T3DModel *model = t3d_model_load("rom:/cath.t3dm");
  const float modelScale = 0.0035f;
  const char* animNames[INST_COUNT] = {"Run", "Walk", "Attack1", "Roll"};

  Instance inst[INST_COUNT];
  for(int i=0; i<INST_COUNT; ++i) {
    inst[i].skel = t3d_skeleton_create_buffered(model, FB_COUNT);
    inst[i].anim = t3d_anim_create(model, animNames[i]);
    t3d_anim_attach(&inst[i].anim, &inst[i].skel);
    inst[i].posX = (i - (INST_COUNT-1) * 0.5f) * 30.0f;
  }

  // skeleton matrices come from a segment, so a single block works for all instances
  rspq_block_begin();
  t3d_model_draw_skinned(model, &inst[0].skel);
  rspq_block_t *dplDraw = rspq_block_end();

  TimeStat statAnim = {};
  TimeStat statSkel = {};
  uint32_t frame = 0;
  int frameIdx = 0;

  for(;;)
  {
    // ======== Update ======== //
    frameIdx = (frameIdx + 1) % FB_COUNT;

    uint32_t ticks = get_ticks();
    for(int i=0; i<INST_COUNT; ++i) {
      t3d_anim_update(&inst[i].anim, DELTA_TIME);
    }
    uint32_t ticksAnim = get_ticks() - ticks;

    ticks = get_ticks();
    for(int i=0; i<INST_COUNT; ++i) {
      t3d_skeleton_update(&inst[i].skel);
    }
    uint32_t ticksSkel = get_ticks() - ticks;

    stat_add(&statAnim, ticksAnim);
    stat_add(&statSkel, ticksSkel);
    if(++frame % AVG_FRAMES == 0) {
      stat_flush(&statAnim);
      stat_flush(&statSkel);
    }

    for(int i=0; i<INST_COUNT; ++i) {
      t3d_mat4fp_from_srt_euler(&modelMatFP[frameIdx * INST_COUNT + i],
        (float[3]){modelScale, modelScale, modelScale},
        (float[3]){0, 0, 0},
        (float[3]){inst[i].posX, 0, 0}
      );
    }

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 200.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0, 1, 0}});

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(30, 30, 40, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    for(int i=0; i<INST_COUNT; ++i) {
      t3d_matrix_push(&modelMatFP[frameIdx * INST_COUNT + i]);
      rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
      t3d_skeleton_use(&inst[i].skel);
      rspq_block_run(dplDraw);
      t3d_matrix_pop(1);
    }

    // ======== Draw (UI) ======== //
    float posY = 20;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, posY, "CPU (%d inst.)  avg / peak", INST_COUNT);
    posY += 12;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, posY, "Anim: %4ldus / %4ldus",
      TICKS_TO_US(statAnim.sum / AVG_FRAMES), TICKS_TO_US(statAnim.peak));
    posY += 10;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, posY, "Skel: %4ldus / %4ldus",
      TICKS_TO_US(statSkel.sum / AVG_FRAMES), TICKS_TO_US(statSkel.peak));

    for(int i=0; i<INST_COUNT; ++i) {
      T3DVec3 screenPos;
      t3d_viewport_calc_viewspace_pos(&viewport, &screenPos, &(T3DVec3){{inst[i].posX, 0, 0}});
      rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, (int)screenPos.v[0] - 16, 214, "%s", animNames[i]);
    }

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}
