#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

#define FB_COUNT 3

color_t get_rainbow_color(float s) {
  float r = fm_sinf(s + 0.0f) * 127.0f + 128.0f;
  float g = fm_sinf(s + 2.0f) * 127.0f + 128.0f;
  float b = fm_sinf(s + 4.0f) * 127.0f + 128.0f;
  return RGBA32(r, g, b, 255);
}

/**
 * Simple example with a 3d-model file created in blender.
 * This uses the builtin model format for loading and drawing a model.
 */
int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, FB_COUNT, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();

  t3d_init((T3DInitParams){});

  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  // Note: this gets DMA'd to the RSP, so it needs to be uncached.
  // If you can't allocate uncached memory, remember to flush the cache after writing to it instead.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP) * FB_COUNT); // allocate one matrix for each framebuffer

  // Also create a buffered viewport to have a distinct matrix for each frame, avoiding corruptions if the CPU is too fast
  // In an actual game make sure to free this viewport via 't3d_viewport_destroy' if no longer needed.
  T3DViewport viewport = t3d_viewport_create_buffered(FB_COUNT);

  const T3DVec3 camPos = {{0,10.0f,40.0f}};
  const T3DVec3 camTarget = {{0,0,0}};

  uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};
  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};

  T3DVec3 lightDirVec = {{-1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  // Load a model-file, this contains the geometry and some metadata
  T3DModel *model = t3d_model_load("rom:/model.t3dm");

  float rotAngle = 0.0f;
  rspq_block_t *dplDraw = NULL;
  int frameIdx = 0;

  for(;;)
  {
    // ======== Update ======== //
    // cycle through FP matrices to avoid overwriting what the RSP may still need to load
    frameIdx = (frameIdx + 1) % FB_COUNT;

    rotAngle -= 0.02f;
    float modelScale = 0.1f;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // slowly rotate model, for more information on matrices and how to draw objects
    // see the example: "03_objects"
    t3d_mat4fp_from_srt_euler(&modelMatFP[frameIdx],
      (float[3]){modelScale, modelScale, modelScale},
      (float[3]){0.0f, rotAngle*0.2f, rotAngle},
      (float[3]){0,0,0}
    );

    // ======== Draw ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(100, 80, 80, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    // you can use the regular rdpq_* functions with t3d.
    // In this example, the colored-band in the 3d-model is using the prim-color,
    // even though the model is recorded, you change it here dynamically.
    rdpq_set_prim_color(get_rainbow_color(rotAngle * 0.42f));

    if(!dplDraw) {
      rspq_block_begin();

      // Draw the model, material settings (e.g. textures, color-combiner) are handled internally
      t3d_model_draw(model);
      t3d_matrix_pop(1);
      dplDraw = rspq_block_end();
    }

    t3d_matrix_push(&modelMatFP[frameIdx]);
    // for the actual draw, you can use the generic rspq-api.
    rspq_block_run(dplDraw);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}
