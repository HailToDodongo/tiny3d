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

const char* TARGET_NAMES[4] = {"Pos", "ScaleV", "ScaleS", "Rot"};

void set_selected_color(bool selected) {
  rdpq_set_prim_color(selected ? RGBA32(0xFF, 0xFF, 0xFF, 0xFF) : RGBA32(0x66, 0x66, 0x66, 0xFF));
}

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

  T3DModel *model = t3d_model_load("rom:/box.t3dm");
  T3DSkeleton skel = t3d_skeleton_create(model);

  // Read out all animations in the model.
  // You probably don't need this if you know the name of the animation.
  // In which case you can create an instance by name directly.
  // @TODO: create anim. instance by name
  uint32_t animCount = t3d_model_get_animation_count(model);
  T3DChunkAnim **anims = malloc(animCount * sizeof(void*));
  t3d_model_get_animations(model, anims);

  rspq_block_begin();
  t3d_model_draw_skinned(model, &skel);
  rspq_block_t *dplDraw = rspq_block_end();

  float rotAngle = -0.4f;
  float rotAngleAdd = 0.0f;
  float colorTimer = 0.0f;
  int activeAnim = 0;

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

    if(joypad.btn.r)rotAngleAdd += 0.01f;
    if(joypad.btn.l)rotAngleAdd -= 0.01f;

    if(btn.c_up)--activeAnim;
    if(btn.c_down)++activeAnim;
    activeAnim = (activeAnim + (int)animCount) % (int)animCount;

    colorTimer += 0.01f;

    rotAngle += rotAngleAdd;
    rotAngleAdd *= 0.9f;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    if(syncPoint)rspq_syncpoint_wait(syncPoint);
    t3d_skeleton_update(&skel);

    t3d_mat4fp_from_srt_euler(modelMatFP,
      (float[3]){0.2f, 0.2f, 0.2f},
      (float[3]){0.0f, rotAngle, 0},
      (float[3]){12,18,0}
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
      rspq_block_run(dplDraw);
      syncPoint = rspq_syncpoint_new();

    t3d_matrix_pop(1);

    // ======== Draw (UI) ======== //
    t3d_debug_print_start();

    // Bone View
    float posX = 12;
    float posY = 12;
    rdpq_set_prim_color(RGBA32(0xAA, 0xAA, 0xFF, 0xFF));
    t3d_debug_print(posX, posY, "Animations:");
    rdpq_set_prim_color(COLOR_BTN_C);
    t3d_debug_print(posX + 90, posY, T3D_DEBUG_CHAR_C_UP T3D_DEBUG_CHAR_C_DOWN); posY += 10;

    for(int i = 0; i < animCount; i++)
    {
      const T3DChunkAnim *anim = anims[i];
      set_selected_color(activeAnim == i);

      t3d_debug_printf(posX, posY, "%s: %.2fs", anim->name, anim->duration);
      posY += 10;
    }

    posY = 120;
    T3DChunkAnim *anim = anims[activeAnim];
    rdpq_set_prim_color(RGBA32(0xAA, 0xAA, 0xFF, 0xFF));
    t3d_debug_printf(posX, posY, "Animation: %s", anim->name);
    posY += 10;
    rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));
    t3d_debug_printf(posX, posY, "Pages: %d (max: %db)", anim->pageCount, anim->maxPageSize);
    posY += 10;
    for(int i = 0; i < anim->pageCount; i++)
    {
      T3DAnimPage *page = &anim->pageTable[i];
      t3d_debug_printf(posX, posY, " 0x%04X: %.2fs %dHz %db", page->dataOffset, page->timeStart, page->sampleRate, page->dataSize);
      posY += 10;
    }

    posY += 10;
    t3d_debug_printf(posX, posY, "Channels: %d", anim->channelCount);
    posY += 10;
    for(int i = 0; i < anim->channelCount; i++)
    {
      T3DAnimChannelMapping *mapping = &anim->channelMappings[i];
      if(mapping->targetType == T3D_ANIM_TARGET_ROTATION) {
        t3d_debug_printf(posX, posY, " Bone[%d] %s", mapping->targetIdx, TARGET_NAMES[mapping->targetType]);
      } else {
        t3d_debug_printf(posX, posY, " Bone[%d] %s[%d] x%.2f %+.2f",
          mapping->targetIdx, TARGET_NAMES[mapping->targetType],
          mapping->attributeIdx, mapping->quantScale, mapping->quantOffset
        );
      }
      posY += 10;
    }

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

