#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3ddebug.h>

color_t get_rainbow_color(float s) {
  float r = fm_sinf(s + 0.0f) * 75.0f + 128.0f;
  float g = fm_sinf(s + 2.0f) * 75.0f + 128.0f;
  float b = fm_sinf(s + 4.0f) * 75.0f + 128.0f;
  return RGBA32(r, g, b, 255);
}
const color_t COLOR_BTN_C = (color_t){0xFF, 0xDD, 0x36, 0xFF};
const color_t COLOR_BTN_A = (color_t){0x30, 0x36, 0xE3, 0xFF};
const color_t COLOR_BTN_B = (color_t){0x39, 0xBF, 0x1F, 0xFF};

void set_selected_color(bool selected) {
  rdpq_set_prim_color(selected ? RGBA32(0xFF, 0xFF, 0xFF, 0xFF) : RGBA32(0x66, 0x66, 0x66, 0xFF));
}

/**
 * Example showing how to load, instantiate and modify a skinned model.
 * This also includes using a bone-matrix to "attach" a model to it.
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
  T3DMat4FP* matrixBoxFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DVec3 camPos = {{0,40.0f,40.0f}};
  T3DVec3 camTarget = {{0,30,0}};

  uint8_t colorAmbient[4] = {0xBB, 0xBB, 0xBB, 0xFF};
  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  T3DModel *model = t3d_model_load("rom:/chicken.t3dm"); // Credits (CC0): https://vertexcat.itch.io/farm-animals-set
  T3DModel *modelBox = t3d_model_load("rom:/box.t3dm");

  // This creates an instance of a skeleton which can then be modified.
  // The model itself stays untouched, and you can create as many instances as you want.
  T3DSkeleton skel = t3d_skeleton_create(model);

  rspq_block_begin();
  t3d_model_draw(modelBox);
  rspq_block_t *dplBox = rspq_block_end();

  rspq_block_begin();
  t3d_model_draw_skinned(model, &skel);
  rspq_block_t *dplDraw = rspq_block_end();

  float rotAngle = -0.4f;
  float rotAngleAdd = 0.0f;
  float colorTimer = 0.0f;
  int activeBone = 0;
  int transformMode = 0;

  // Bones can be queried by name, the index should be cached for performance reasons.
  int attachedBone = t3d_skeleton_find_bone(&skel, "Top");

  // Since we double buffer the screen, matrices could update mid-render.
  // You can either double-buffer the skeleton matrices, or use a sync-point, in this example the latter is used.
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

    if(btn.a)++transformMode;
    if(btn.b)--transformMode;
    transformMode = (transformMode + 3) % 3;

    if(joypad.btn.r)rotAngleAdd += 0.01f;
    if(joypad.btn.l)rotAngleAdd -= 0.01f;

    switch(transformMode) {
      case 0:
        skel.bones[activeBone].scale.v[0] += joypad.stick_y * SPEED_SCALE;
        skel.bones[activeBone].scale.v[1] += joypad.stick_y * SPEED_SCALE;
        skel.bones[activeBone].scale.v[2] += joypad.stick_y * SPEED_SCALE;
      break;
      case 1:
        t3d_quat_rotate_euler(&skel.bones[activeBone].rotation, (float[3]){1,0,0}, joypad.stick_x * SPEED_ROT);
        if(joypad.btn.z) {
          t3d_quat_rotate_euler(&skel.bones[activeBone].rotation, (float[3]){0,0,1}, joypad.stick_y * SPEED_ROT);
        } else {
          t3d_quat_rotate_euler(&skel.bones[activeBone].rotation, (float[3]){0,1,0}, joypad.stick_y * SPEED_ROT);
        }
      break;
      case 2:
        skel.bones[activeBone].position.v[0] += joypad.stick_x * SPEED_MOVE;
        skel.bones[activeBone].position.v[joypad.btn.z ? 1 : 2] += joypad.stick_y * SPEED_MOVE;
      break;
    }

    if(btn.c_up)--activeBone;
    if(btn.c_down)++activeBone;
    activeBone = (activeBone+skel.skeletonRef->boneCount) % skel.skeletonRef->boneCount;

    if(btn.c_right)++attachedBone;
    if(btn.c_left && attachedBone >= 0)--attachedBone;
    if(attachedBone >= 0)attachedBone = attachedBone % skel.skeletonRef->boneCount;

    colorTimer += 0.01f;

    rotAngle += rotAngleAdd;
    rotAngleAdd *= 0.9f;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // Before touching the skeleton matrices, we wait for the RSP to be done using it
    if(syncPoint)rspq_syncpoint_wait(syncPoint);
    // This function updates any matrices that need re-calculating after scale/rot/pos changes
    t3d_skeleton_update(&skel);

    t3d_mat4fp_from_srt_euler(modelMatFP,
      (float[3]){0.45f, 0.45f, 0.45f},
      (float[3]){0.0f, rotAngle, 0},
      (float[3]){12,5,0}
    );
    t3d_mat4fp_from_srt_euler(matrixBoxFP,
      (float[3]){0.08f, 0.08f, 0.08f},
      (float[3]){colorTimer*1.5f,colorTimer*1.2f,colorTimer*1.7f},
      (float[3]){0,10,0}
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

    // Draw Chicken & Box
    t3d_matrix_push(modelMatFP);

      rdpq_set_prim_color(get_rainbow_color(colorTimer));
      rspq_block_run(dplDraw); // skinned mesh is already recorded, use a normal draw here
      rdpq_set_prim_color(RGBA32(255, 255, 255, 255));

      if(attachedBone >= 0) {
        // to attach another model, simply use a bone form the skeleton:
        t3d_matrix_push(&skel.boneMatricesFP[attachedBone]);
        t3d_matrix_push(matrixBoxFP); // apply local matrix of the model
          rspq_block_run(dplBox);
        t3d_matrix_pop(2);
      }
      syncPoint = rspq_syncpoint_new(); // create a sync-point to let the CPU know we are done with using the matrices

    t3d_matrix_pop(1);

    // ======== Draw (UI) ======== //
    t3d_debug_print_start();

    // Bone View
    float posX = 12;
    float posY = 12;
    rdpq_set_prim_color(RGBA32(0xAA, 0xAA, 0xFF, 0xFF));
    t3d_debug_print(posX, posY, "Bones: ");
    rdpq_set_prim_color(COLOR_BTN_C);
    t3d_debug_print(posX + 50, posY, T3D_DEBUG_CHAR_C_UP T3D_DEBUG_CHAR_C_DOWN); posY += 10;

    for(int i = 0; i < skel.skeletonRef->boneCount; i++)
    {
      const T3DBone *bone = &skel.bones[i];
      set_selected_color(activeBone == i);

      const T3DChunkBone *boneRef = &skel.skeletonRef->bones[i];
      t3d_debug_printf(posX, posY, "%.2d:", i);
      t3d_debug_print(posX + 24 + (boneRef->depth * 8), posY, boneRef->name);
      posY += 10;
    }
    posY += 8;

    // Attached bone
    rdpq_set_prim_color(RGBA32(0xAA, 0xAA, 0xFF, 0xFF));
    t3d_debug_print(posX, posY, "Box:");
    rdpq_set_prim_color(COLOR_BTN_C);
    t3d_debug_print(posX + 34, posY, T3D_DEBUG_CHAR_C_LEFT T3D_DEBUG_CHAR_C_RIGHT);
    posY += 10;

    rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));
    t3d_debug_printf(posX, posY, "Attached: %s", attachedBone >= 0 ? skel.skeletonRef->bones[attachedBone].name : "-None-");
    posY += 18;

    // Transform mode
    posY = 240 - 50;
    rdpq_set_prim_color(RGBA32(0xAA, 0xAA, 0xFF, 0xFF));
    t3d_debug_print(posX, posY, "Mode:");
    rdpq_set_prim_color(COLOR_BTN_A); t3d_debug_print(posX+42, posY, T3D_DEBUG_CHAR_A);
    rdpq_set_prim_color(COLOR_BTN_B); t3d_debug_print(posX+52, posY, T3D_DEBUG_CHAR_B);
    posY += 10;

    // Bone Attributes (SRT)
    set_selected_color(transformMode == 0);
    t3d_debug_printf(posX, posY, "S: %.2f %.2f %.2f", skel.bones[activeBone].scale.v[0], skel.bones[activeBone].scale.v[1], skel.bones[activeBone].scale.v[2]);

    posY += 10;
    set_selected_color(transformMode == 1);
    t3d_debug_printf(posX, posY, "R: %.2f %.2f %.2f %.2f", skel.bones[activeBone].rotation.v[0], skel.bones[activeBone].rotation.v[1], skel.bones[activeBone].rotation.v[2], skel.bones[activeBone].rotation.v[3]);

    posY += 10;
    set_selected_color(transformMode == 2);
    t3d_debug_printf(posX, posY, "T: %.2f %.2f %.2f", skel.bones[activeBone].position.v[0], skel.bones[activeBone].position.v[1], skel.bones[activeBone].position.v[2]);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

