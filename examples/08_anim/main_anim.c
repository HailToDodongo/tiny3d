#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

const color_t COLOR_BTN_C = (color_t){0xFF, 0xDD, 0x36, 0xFF};
const color_t COLOR_BTN_A = (color_t){0x30, 0x36, 0xE3, 0xFF};
const color_t COLOR_BTN_B = (color_t){0x39, 0xBF, 0x1F, 0xFF};

const char TARGET_NAMES[4] = {'P', 's', 'S', 'R'};

void set_selected_color(bool selected) {
  rdpq_set_prim_color(selected ? RGBA32(0xFF, 0xFF, 0xFF, 0xFF) : RGBA32(0x66, 0x66, 0x66, 0xFF));
}

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

/**
 * Example showing how to load, instantiate and play skeletal-animations.
 */
int main()
{
  debug_init_isviewer();
  debug_init_usblog();
  asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  surface_t depthBuffer = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

  rdpq_init();
  joypad_init();

  t3d_init((T3DInitParams){});
  t3d_debug_print_init();
  T3DViewport viewport = t3d_viewport_create();

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DVec3 camPos = {{0,40.0f,40.0f}};
  T3DVec3 camTarget = {{0,30,0}};

  uint8_t colorAmbient[4] = {0xBB, 0xBB, 0xBB, 0xFF};
  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  #define MODEL_COUNT 5
  ModelAnim modelData[MODEL_COUNT] = {
    { .model = t3d_model_load("rom:/cath.t3dm"), .scale = 0.0035f },
    { .model = t3d_model_load("rom:/animTest.t3dm"), .scale = 0.08f },
    { .model = t3d_model_load("rom:/box.t3dm"), .scale = 0.08f },
    { .model = t3d_model_load("rom:/box2.t3dm"), .scale = 0.08f },
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
  //T3DSkeleton skel = t3d_skeleton_create(model);
  //T3DSkeleton skelBlend = t3d_skeleton_clone(&skel, false); // <- has no matrices

  // Read out all animations in the model.
  // You probably don't need this if you know the name of the animation.
  // In which case you can create an instance by name directly.
  /*uint32_t animCount = t3d_model_get_animation_count(model);
  T3DChunkAnim **anims = malloc(animCount * sizeof(void*));
  t3d_model_get_animations(model, anims);

  T3DAnim *animInst = malloc(animCount * sizeof(T3DAnim));
  for(int i = 0; i < animCount; i++) {
    animInst[i] = t3d_anim_create(model, anims[i]->name);
    t3d_anim_attach(&animInst[i], &skel);
  }*/
/*
  rspq_block_begin();
  t3d_model_draw_skinned(model, &skel);
  //t3d_model_draw(model);
  rspq_block_t *dplDraw = rspq_block_end();*/

  int modelIdx = 0;
  float colorTimer = 0.0f;
  int activeAnim = 5;
  int activeBlendAnim = -1;
  float lastTime = get_time_s() - (1.0f / 60.0f);
  float blendFactor = 0.0f;
  float timeCursor = 0.5f;

  rspq_syncpoint_t syncPoint = 0;
  bool play = true;

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
    if(btn.c_up)--activeAnim;
    if(btn.c_down)++activeAnim;
    if(btn.c_left)--modelIdx;
    if(btn.c_right)++modelIdx;
    //if(btn.a)play = !play;

    timeCursor += (float)joypad.stick_x * 0.0001f;

    modelIdx = (modelIdx + MODEL_COUNT) % MODEL_COUNT;

    ModelAnim *md = &modelData[modelIdx];

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
    if(btn.a) {
      t3d_anim_set_time(&md->animInst[activeAnim], timeCursor);
    }

    if(joypad.btn.c_left)blendFactor -= 0.0075f;
    if(joypad.btn.c_right)blendFactor += 0.0075f;

    if(blendFactor > 1.0f)blendFactor = 1.0f;
    if(blendFactor < 0.0f)blendFactor = 0.0f;

    activeAnim = (activeAnim + (int)md->animCount) % (int)md->animCount;

    if(lastAnim != activeAnim || !play) {
      t3d_skeleton_reset(&md->skel);
    }

    md->animInst[activeAnim].speed += (float)joypad.stick_y * 0.0001f;
    if(md->animInst[activeAnim].speed < 0.0f)md->animInst[activeAnim].speed = 0.0f;

    if(timeCursor < 0.0f)timeCursor = md->animInst[activeAnim].animRef->duration;
    if(timeCursor > md->animInst[activeAnim].animRef->duration)timeCursor = 0.0f;

    float newTime = get_time_s();
    float deltaTime = newTime - lastTime;
    lastTime = newTime;

    colorTimer += 0.01f;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    if(play) {
      t3d_anim_update(&md->animInst[activeAnim], deltaTime);

      if(activeBlendAnim >= 0) {
        t3d_anim_update(&md->animInst[activeBlendAnim], deltaTime);
        t3d_skeleton_blend(&md->skel, &md->skelBlend, &md->skel, blendFactor);
      }
    }

    if(syncPoint)rspq_syncpoint_wait(syncPoint);
    t3d_skeleton_update(&md->skel);

    t3d_mat4fp_from_srt_euler(modelMatFP,
      (float[3]){md->scale, md->scale, md->scale},
      (float[3]){0.0f, 0, 0},
      (float[3]){25,2,0}
    );

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), &depthBuffer);
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
    t3d_debug_print_start();

    // Bone View
    float posX = 12;
    float posY = 12;
    rdpq_set_prim_color(RGBA32(0xAA, 0xAA, 0xFF, 0xFF));
    t3d_debug_printf(posX, posY, "Animations:", blendFactor);
    posY += 10;

    for(int i = 0; i < md->animCount; i++)
    {
      const T3DChunkAnim *anim = md->anims[i];
      set_selected_color(activeAnim == i);

      if(activeBlendAnim == i) {
        rdpq_set_prim_color(COLOR_BTN_B);
        t3d_debug_printf(posX, posY, "%s: %.2fs (%d%%)", anim->name, anim->duration, (int)((1.0f-blendFactor) * 100));
      } else {
        if(activeAnim == i && activeBlendAnim >= 0) {
          t3d_debug_printf(posX, posY, "%s: %.2fs (%d%%)", anim->name, anim->duration, (int)(blendFactor * 100));
        } else {
          t3d_debug_printf(posX, posY, "%s: %.2fs", anim->name, anim->duration);
        }
      }
      posY += 10;
    }

    posY += 10;
    T3DChunkAnim *anim = md->anims[activeAnim];
    rdpq_set_prim_color(RGBA32(0xAA, 0xAA, 0xFF, 0xFF));
    t3d_debug_printf(posX, posY, "Animation: %s", anim->name);
    posY += 10;
    rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));

    t3d_debug_printf(posX, posY, "SData: %s", anim->filePath);
    posY += 10;
    t3d_debug_printf(posX, posY, "KF-Count: %d", anim->keyframeCount);
    posY += 10;

    rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));
    t3d_debug_printf(posX, posY, "Channels: %d+%d", anim->channelsQuat, anim->channelsScalar);
    /*posY += 10;
    for(int i = 0; i < (anim->channelsQuat + anim->channelsScalar); i++)
    {
      T3DAnimChannelMapping *mapping = &anim->channelMappings[i];
      if(mapping->targetType == T3D_ANIM_TARGET_ROTATION) {
        T3DAnimTargetQuat *target = &md->animInst[activeAnim].targetsQuat[i];
        t3d_debug_printf(posX, posY, " B[%d] %c, time: %.2f-%.2fs (%.2fs)", mapping->targetIdx, TARGET_NAMES[mapping->targetType], target->base.timeStart, target->base.timeEnd, target->base.timeNextKF);
      } else {
        t3d_debug_printf(posX, posY, " B[%d] %c.%d x%.2f %+.2f",
          mapping->targetIdx, TARGET_NAMES[mapping->targetType],
          mapping->attributeIdx, mapping->quantScale * 0xFFFF, mapping->quantOffset
        );
      }
      posY += 10;
    }*/

    posY += 30;
    t3d_debug_printf(posX, posY, "Speed: %.2fx", md->animInst[activeAnim].speed);

    posY += 30;
    if(play) {
      t3d_debug_printf(posX, posY, "Time: %.2fs / %.2fs", md->animInst[activeAnim].time, anim->duration);
    }
    posY -= 14;

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

