#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

/**
 * Demo for a model and animation viewer.
 * This is more intended for debugging and testing purposes of t3d itself.
 * However, it does show how you can access animation data dynamically.
 *
 * Controls:
 * - L/R      : Switch between models
 * - C-U/C-D  : Switch between animations
 * - Stick L/R: Change time cursor
 * - Stick U/D: Change animation speed
 *
 * - B    : (De-)Select current animation for blending
 * - C-L/R: Change blend factor
 *
 * - Start: Pause/Resume current animation
 * - Z    : Toggle looping of current animation
 * - A    : Set time cursor to current animation time
 */
float get_time_s() {
  return (float)((double)get_ticks_us() / 1000000.0);
}

typedef struct {
  T3DModel *model;
  T3DSkeleton skel;
  T3DSkeleton skelBlend;
  T3DAnim *animInst;
  float scale;
  rspq_block_t *dplDraw;
  uint32_t animCount;
  T3DChunkAnim **anims;
} ModelAnim;

#define STRINGIFY(x) #x
#define STYLE(id) "^0" STRINGIFY(id)
#define STYLE_TITLE 1
#define STYLE_GREY 2
#define STYLE_GREEN 3

int main()
{
  debug_init_isviewer();
  debug_init_usblog();
  asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  joypad_init();

  t3d_init((T3DInitParams){});

  rdpq_font_t* fnt = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
  rdpq_font_style(fnt, STYLE_TITLE, &(rdpq_fontstyle_t){RGBA32(0xAA, 0xAA, 0xFF, 0xFF)});
  rdpq_font_style(fnt, STYLE_GREY,  &(rdpq_fontstyle_t){RGBA32(0x66, 0x66, 0x66, 0xFF)});
  rdpq_font_style(fnt, STYLE_GREEN, &(rdpq_fontstyle_t){RGBA32(0x39, 0xBF, 0x1F, 0xFF)});
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, fnt);

  T3DViewport viewport = t3d_viewport_create();

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DVec3 camPos = {{0,40.0f,40.0f}};
  T3DVec3 camTarget = {{0,30,0}};

  uint8_t colorAmbient[4] = {0xBB, 0xBB, 0xBB, 0xFF};
  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  #define MODEL_COUNT 3
  ModelAnim modelData[MODEL_COUNT] = {
    { .model = t3d_model_load("rom:/cath.t3dm"), .scale = 0.0035f }, // "catherine.blend" Model from: https://github.com/buu342/N64-Sausage64
    { .model = t3d_model_load("rom:/animTest.t3dm"), .scale = 0.08f },
    { .model = t3d_model_load("rom:/enemyPlant00.t3dm"), .scale = 0.3f }
  };

  for(int i = 0; i < MODEL_COUNT; i++) {
    modelData[i].skel = t3d_skeleton_create(modelData[i].model);
    modelData[i].skelBlend = t3d_skeleton_clone(&modelData[i].skel, false); // <- has no matrices
    modelData[i].animCount = t3d_model_get_animation_count(modelData[i].model);
    modelData[i].anims = malloc(modelData[i].animCount * sizeof(void*));

    t3d_model_get_animations(modelData[i].model, modelData[i].anims);

    modelData[i].animInst = malloc(modelData[i].animCount * sizeof(T3DAnim));
    for(int j = 0; j < t3d_model_get_animation_count(modelData[i].model); j++) {
      modelData[i].animInst[j] = t3d_anim_create(modelData[i].model, t3d_model_get_animation(modelData[i].model, modelData[i].anims[j]->name)->name);
      t3d_anim_attach(&modelData[i].animInst[j], &modelData[i].skel);
    }

    rspq_block_begin();
    t3d_model_draw_skinned(modelData[i].model, &modelData[i].skel);
    modelData[i].dplDraw = rspq_block_end();
  }

  int modelIdx = 0;
  int activeAnim = 5;
  int activeBlendAnim = -1;
  float lastTime = get_time_s() - (1.0f / 60.0f);
  float blendFactor = 0.0f;
  float timeCursor = 0.5f;

  rspq_syncpoint_t syncPoint = 0;

  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    const float SPEED_ROT = 0.0008f;
    const float SPEED_MOVE = 0.008f;
    const float SPEED_SCALE = 0.0003f;

    int lastAnim = activeAnim;

    if(btn.l)--modelIdx;
    if(btn.r)++modelIdx;
    modelIdx = (modelIdx + MODEL_COUNT) % MODEL_COUNT;
    ModelAnim *md = &modelData[modelIdx];

    if(btn.c_up)--activeAnim;
    if(btn.c_down)++activeAnim;
    activeAnim = (activeAnim + (int)md->animCount) % (int)md->animCount;

    if(btn.start)t3d_anim_set_playing(&md->animInst[activeAnim], !md->animInst[activeAnim].isPlaying);
    if(btn.z)t3d_anim_set_looping(&md->animInst[activeAnim], !md->animInst[activeAnim].isLooping);

    if(joypad.btn.c_left)blendFactor -= 0.0075f;
    if(joypad.btn.c_right)blendFactor += 0.0075f;
    if(blendFactor > 1.0f)blendFactor = 1.0f;
    if(blendFactor < 0.0f)blendFactor = 0.0f;
    timeCursor += (float)joypad.stick_x * 0.0001f;

    if(btn.b) {
      if(activeBlendAnim >= 0) {
        t3d_anim_attach(&md->animInst[activeBlendAnim], &md->skel);
      }
      activeBlendAnim = (activeBlendAnim == activeAnim) ? -1 : activeAnim;

      if(activeBlendAnim >= 0) {
        t3d_skeleton_reset(&md->skelBlend);
        t3d_anim_attach(&md->animInst[activeBlendAnim], &md->skelBlend);
      }
    }
    if(activeBlendAnim >= md->animCount)activeBlendAnim = -1;

    if(btn.a)t3d_anim_set_time(&md->animInst[activeAnim], timeCursor);

    if(lastAnim != activeAnim) {
      t3d_skeleton_reset(&md->skel);
    }

    md->animInst[activeAnim].speed += (float)joypad.stick_y * 0.0001f;
    if(md->animInst[activeAnim].speed < 0.0f)md->animInst[activeAnim].speed = 0.0f;

    if(timeCursor < 0.0f)timeCursor = md->animInst[activeAnim].animRef->duration;
    if(timeCursor > md->animInst[activeAnim].animRef->duration)timeCursor = 0.0f;

    float newTime = get_time_s();
    float deltaTime = newTime - lastTime;
    lastTime = newTime;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    t3d_anim_update(&md->animInst[activeAnim], deltaTime);

    if(activeBlendAnim >= 0) {
      t3d_anim_update(&md->animInst[activeBlendAnim], deltaTime);
      t3d_skeleton_blend(&md->skel, &md->skelBlend, &md->skel, blendFactor);
    }

    if(syncPoint)rspq_syncpoint_wait(syncPoint);
    t3d_skeleton_update(&md->skel);

    t3d_mat4fp_from_srt_euler(modelMatFP,
      (float[3]){md->scale, md->scale, md->scale},
      (float[3]){0.0f, 0, 0},
      (float[3]){25,2,0}
    );

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(10, 10, 10, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    t3d_matrix_push(modelMatFP);
      rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
      rspq_block_run(md->dplDraw);
    t3d_matrix_pop(1);

    syncPoint = rspq_syncpoint_new();

    // ======== Draw (UI) ======== //
    float posX = 16;
    float posY = 20;

    rdpq_sync_pipe();
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, STYLE(STYLE_TITLE) "[L/R] Model: %d/%d", modelIdx+1, MODEL_COUNT);
    posY += 10;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, STYLE(STYLE_TITLE) "[C] Animations:");
    posY += 10;

    for(int i = 0; i < md->animCount; i++)
    {
      const T3DChunkAnim *anim = md->anims[i];
      int style = (activeAnim == i) ? 0 : STYLE_GREY;

      if(activeBlendAnim == i) {
        style = STYLE_GREEN;
        rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "^0%d" "%s: %.2fs (%d%%)", style, anim->name, anim->duration, (int)((1.0f-blendFactor) * 100));
      } else {
        if(activeAnim == i && activeBlendAnim >= 0) {
          rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "^0%d" "%s: %.2fs (%d%%)", style, anim->name, anim->duration, (int)(blendFactor * 100));
        } else {
          rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "^0%d" "%s: %.2fs", style, anim->name, anim->duration);
        }
      }
      posY += 10;
    }

    posY += 10;
    T3DChunkAnim *anim = md->anims[activeAnim];
    rdpq_set_prim_color(RGBA32(0xAA, 0xAA, 0xFF, 0xFF));
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, STYLE(STYLE_TITLE) "Animation: %s", anim->name);
    posY += 10;
    rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "SData: %s", anim->filePath);
    posY += 10;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "KF-Count: %ld", anim->keyframeCount);
    posY += 10;

    rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Channels: %d+%d", anim->channelsQuat, anim->channelsScalar);
    posY += 24;

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Speed: %.2fx", md->animInst[activeAnim].speed);
    posY += 30;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Time: %.2fs / %.2fs %c", md->animInst[activeAnim].time, anim->duration,
      md->animInst[activeAnim].isLooping ? 'L' : '-'
    );
    posY -= 24;

    float barWidth = 150.0f;
    float barHeight = 10.0f;
    float timeScale = barWidth / anim->duration;

    // Timeline
    rdpq_set_mode_fill(RGBA32(0x00, 0x00, 0x00, 0xFF)); // backdrop
    rdpq_fill_rectangle(posX-2, posY-2, posX + barWidth+2, posY + barHeight+2);

    rdpq_set_fill_color(RGBA32(0x33, 0x33, 0x33, 0xFF)); // background
    rdpq_fill_rectangle(posX, posY, posX + barWidth, posY + barHeight);

    rdpq_set_fill_color(RGBA32(0xAA, 0xAA, 0xAA, 0xFF)); // progress bar
    rdpq_fill_rectangle(posX, posY, posX + (md->animInst[activeAnim].time * timeScale), posY + barHeight);

    rdpq_set_fill_color(RGBA32(0x55, 0x55, 0xFF, 0xFF)); // cursor
    rdpq_fill_rectangle(posX + (timeCursor * timeScale), posY-2, posX + (timeCursor * timeScale) + 2, posY + barHeight+2);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

