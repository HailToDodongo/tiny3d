#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

typedef struct {
  color_t color;
  T3DVec3 dir;
} DirLight;

/**
 * Example showing off the lighting API.
 */
int main()
{
	debug_init_isviewer();
	debug_init_usblog();

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);
  rdpq_init();
  //rdpq_debug_start();
  //rdpq_debug_log(true);

  t3d_init(); // Init library itself

  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  t3d_mat4_identity(&modelMat);
  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP* lightMatFP = malloc_uncached(sizeof(T3DMat4FP) * 4);

  T3DVec3 camPos = {{40.0f,15.0f,0}};
  const T3DVec3 camTarget = {{0,0,0}};

  DirLight dirLights[4] = {
    {.color = {0xFF, 0x00, 0x00, 0xFF}, .dir = {{ 1.0f,  1.0f, 0.0f}}},
    {.color = {0x00, 0xFF, 0x00, 0xFF}, .dir = {{-1.0f,  1.0f, 0.0f}}},
    {.color = {0x00, 0x00, 0xFF, 0xFF}, .dir = {{ 0.0f, -1.0f, 0.0f}}},
    {.color = {0x50, 0x50, 0x50, 0xFF}, .dir = {{ 0.0f,  0.0f, 1.0f}}}
  };
  uint8_t colorAmbient[4] = {0, 0, 0, 0xFF};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 0.0f}};
  t3d_vec3_norm(&lightDirVec);

  // Load a model-file, this contains the geometry and some metadata
  T3DModel *model = t3d_model_load("rom:/model.t3dm");
  T3DModel *modelLight = t3d_model_load("rom:/light.t3dm");

  rspq_block_begin();
  t3d_matrix_set_mul(modelMatFP, 1, 0);
  t3d_model_draw(model);
  rspq_block_t *dplModel = rspq_block_end();

  rspq_block_begin();
  t3d_model_draw(modelLight);
  rspq_block_t *dplLight = rspq_block_end();

  float rotAngle = 0.0f;

  for(;;)
  {
    // ======== Update ======== //
    rotAngle += 0.02f;
    float modelScale = 0.08f;

    // Model Matrix
    t3d_mat4_identity(&modelMat);
    t3d_mat4_rotate(&modelMat, &(T3DVec3){{0.0f, 0.0f, 1.0f}}, rotAngle);
    t3d_mat4_scale(&modelMat, modelScale, modelScale, modelScale);
    t3d_mat4_translate(&modelMat,
      dirLights[0].dir.v[0] * 2.0f,
      dirLights[1].dir.v[1] * 2.0f,
      dirLights[0].dir.v[2] * 2.0f
    );
    t3d_mat4_to_fixed(modelMatFP, &modelMat);

    // Lamp Model Matrix
    for(int i = 0; i < 4; i++) {
      // rotate light around
      float lightRotSpeed = ((1.0f+i)*0.42f);
      dirLights[i].dir.v[0] = cosf(rotAngle * lightRotSpeed + i * 1.6f);
      dirLights[i].dir.v[1] = sinf(rotAngle * lightRotSpeed + i * 1.6f);
      dirLights[i].dir.v[2] = 0.0f;

      if(i % 2 == 0) {
        dirLights[i].dir.v[2] = dirLights[i].dir.v[1];
        dirLights[i].dir.v[1] = 0.0f;
      }

      t3d_mat4_identity(&modelMat);
      t3d_mat4_rotate(&modelMat, &dirLights[i].dir, 180.0f);
      t3d_mat4_scale(&modelMat, 0.02f, 0.02f, 0.02f);
      t3d_mat4_translate(&modelMat,
        dirLights[i].dir.v[0] * 20.0f + i,
        dirLights[i].dir.v[1] * 20.0f + i,
        dirLights[i].dir.v[2] * 20.0f + i
      );
      t3d_mat4_to_fixed(&lightMatFP[i], &modelMat);
    }

    // ======== Draw ======== //
    t3d_frame_start(); // call this once per frame at the beginning of your draw function

    t3d_screen_set_size(display_get_width(), display_get_height(), 2, false);
    t3d_screen_clear_color(RGBA32(25, 10, 40, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_count(4);

    t3d_projection_perspective(T3D_DEG_TO_RAD(65.0f), 10.0f, 250.0f);
    t3d_camera_look_at(&camPos, &camTarget); // convenience function to set camera matrix and related settings

    t3d_light_set_ambient(colorAmbient);

    for(int i = 0; i < 4; i++) {
      t3d_light_set_directional(i, &dirLights[i].color.r, &dirLights[i].dir);
      t3d_matrix_set_mul(&lightMatFP[i], 1, 0);

      rdpq_set_prim_color(dirLights[i].color);
      rspq_block_run(dplLight);
    }

    rspq_block_run(dplModel);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

