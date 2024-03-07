#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>


/**
 * Simple example with a 3d-model file created in blender.
 * This uses the builtin model format for loading and drawing a model.
 */
int main()
{
	debug_init_isviewer();
	debug_init_usblog();

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  rdpq_init();
  //rdpq_debug_start();
  //rdpq_debug_log(true);

  t3d_init(); // Init library itself

  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  T3DMat4 modelMatGear;
  T3DMat4 modelMatRot;
  t3d_mat4_identity(&modelMat);
  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DVec3 camPos = {{0,10.0f,40.0f}};

  const T3DVec3 camTarget = {{0,0,0}};

  uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};
  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  // Load a model-file, this contains the geometry and some metadata
  T3DModel *model = t3d_model_load("rom:/model.t3dm");

  float rotAngle = 0.0f;
  T3DVec3 rotAxis = {{0.0f, 0.0f, 1.0f}};
  T3DVec3 rotAxisB = {{0.0f, 1.0f, 0.0f}};

  rspq_block_t *dplDraw = NULL;

  for(;;)
  {
    // ======== Update ======== //
    rotAngle += 0.02f;
    float modelScale = 0.1f;

    // Model-Matrix, t3d offers some basic matrix functions
    t3d_mat4_identity(&modelMatGear);
    t3d_mat4_rotate(&modelMatGear, &rotAxis, rotAngle);
    t3d_mat4_scale(&modelMatGear, modelScale, modelScale, modelScale);

    t3d_mat4_identity(&modelMatRot);
    t3d_mat4_rotate(&modelMatRot, &rotAxisB, rotAngle * 0.2f);
    t3d_mat4_mul(&modelMat, &modelMatRot, &modelMatGear);

    t3d_mat4_to_fixed(modelMatFP, &modelMat);

    // ======== Draw ======== //
    t3d_frame_start(); // call this once per frame at the beginning of your draw function

    t3d_screen_set_size(display_get_width(), display_get_height(), 1, true);
    t3d_screen_clear_color(RGBA32(100, 80, 80, 0xFF));
    t3d_screen_clear_depth();

    t3d_projection_perspective(T3D_DEG_TO_RAD(85.0f), 10.0f, 400.0f);
    t3d_camera_look_at(&camPos, &camTarget); // convenience function to set camera matrix and related settings

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_TEXTURED | T3D_FLAG_DEPTH | T3D_FLAG_CULL_BACK);

    rdpq_set_prim_color(RGBA32(255, 200, 0, 255));

    if(!dplDraw) {
      rspq_block_begin();

      t3d_matrix_set_mul(modelMatFP, 1, 0);
      // Draw the model, material settings (e.g. color-combiner) are handled internally
      t3d_model_draw(model);
      dplDraw = rspq_block_end();
    }

    rspq_block_run(dplDraw);
    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

