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
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);

  rdpq_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();

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

  T3DModel *model = t3d_model_load("rom:/model.t3dm");
  T3DModel *modelLight = t3d_model_load("rom:/light.t3dm");

  rspq_block_begin();
  t3d_matrix_push(modelMatFP);
  t3d_model_draw(model);
  t3d_matrix_pop(1);
  rspq_block_t *dplModel = rspq_block_end();

  rspq_block_begin();
  t3d_model_draw(modelLight);
  rspq_block_t *dplLight = rspq_block_end();

  float rotAngle = 0.0f;
  float lightCountTimer = 0.5f;

  for(;;)
  {
    // ======== Update ======== //
    rotAngle += 0.02f;
    float modelScale = 0.08f;
    lightCountTimer += 0.003f;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(65.0f), 10.0f, 250.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // Model Matrix
    t3d_mat4fp_from_srt_euler(modelMatFP,
      (float[3]){modelScale, modelScale, modelScale},
      (float[3]){0.0f, rotAngle*0.2f, rotAngle},
      (float[3]){0,0,0}
    );

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

      t3d_mat4fp_from_srt_euler(&lightMatFP[i],
        (float[3]){0.02f, 0.02f, 0.02f},
        dirLights[i].dir.v,
        (float[3]){
          dirLights[i].dir.v[0] * 20.0f + i,
          dirLights[i].dir.v[1] * 20.0f + i,
          dirLights[i].dir.v[2] * 20.0f + i
        }
      );
    }

    // cycle through 0-4 lights over time
    int lightCount = fm_sinf(lightCountTimer) * 5.0f;
    lightCount = abs(lightCount);
    if(lightCount > 4)lightCount = 4;

    // ======== Draw ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start(); // call this once per frame at the beginning of your draw function
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(25, 10, 40, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_count(lightCount);

    t3d_light_set_ambient(colorAmbient);

    t3d_matrix_push_pos(1);
    for(int i = 0; i < lightCount; i++) {
      t3d_light_set_directional(i, &dirLights[i].color.r, &dirLights[i].dir);

      t3d_matrix_set(&lightMatFP[i], true);

      rdpq_set_prim_color(dirLights[i].color);
      rspq_block_run(dplLight);
    }
    t3d_matrix_pop(1);

    rspq_block_run(dplModel);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

