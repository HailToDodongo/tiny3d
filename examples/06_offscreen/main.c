#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

/**
 * Showcase for offscreen rendering.
 * This renders a scene into a texture which then can be used by 3D models.
 */

#define OFFSCREEN_SIZE 80

// This is a callback for t3d_model_draw_custom, it is used when a texture in a model is set to dynamic/"reference"
void dynamic_tex_cb(void* userData, const T3DMaterial* material, rdpq_texparms_t *tileParams, rdpq_tile_t tile, uint32_t texReference) {
  if(tile != TILE0)return; // this callback can happen 2 times per mesh, you are allowed to skip calls

  surface_t *offscreenSurf = (surface_t*)userData;
  rdpq_sync_tile();

  int sHalf = OFFSCREEN_SIZE / 2;
  int sFull = OFFSCREEN_SIZE;

  // upload a slice of the offscreen-buffer, the screen in the TV model is split into 4 materials for each section
  // if you are working with a small enough single texture, you can ofc use a normal sprite upload.
  // the quadrant is determined by the texture reference set in fast64, which can be used as an arbitrary value
  switch(texReference) {
    case 1: rdpq_tex_upload_sub(TILE1, offscreenSurf, NULL,  0,     0,     sHalf, sHalf); break;
    case 2: rdpq_tex_upload_sub(TILE1, offscreenSurf, NULL,  sHalf, 0,     sFull, sHalf); break;
    case 3: rdpq_tex_upload_sub(TILE1, offscreenSurf, NULL,  0,     sHalf, sHalf, sFull); break;
    case 4: rdpq_tex_upload_sub(TILE1, offscreenSurf, NULL,  sHalf, sHalf, sFull, sFull); break;
  }
}

static float random_flicker(float time) {
  float flickrMain = fm_sinf(time) * 0.5f + 0.5f;
  float flickrSub = fm_sinf(time * 0.5f) * 0.5f + 0.5f;

  flickrMain = (flickrMain + flickrSub) * 0.5f;
  return 1.0f - fminf(flickrMain, 1.0f);
}

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  // Allocate our normal screen and depth buffer...
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  surface_t depthBuffer = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

  //... then a smaller additional buffer (color + depth) for offscreen rendering
  surface_t offscreenSurf = surface_alloc(FMT_RGBA16, OFFSCREEN_SIZE, OFFSCREEN_SIZE);
  surface_t offscreenSurfZ = surface_alloc(FMT_RGBA16, OFFSCREEN_SIZE, OFFSCREEN_SIZE);

  rdpq_init();
  //rdpq_debug_start();

  t3d_init();
  t3d_debug_print_init();

  // Like in the demo before we can create multiple viewports.
  // This time however we have to match the offscreen one to the size of the offscreen buffer
  T3DViewport viewport = t3d_viewport_create();
  T3DViewport viewportOffscreen = t3d_viewport_create();
  t3d_viewport_set_area(&viewportOffscreen, 0, 0, OFFSCREEN_SIZE, OFFSCREEN_SIZE);

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP* modelTargetMatFP = malloc_uncached(sizeof(T3DMat4FP));

  const T3DVec3 camPos = {{0,5.0f,40.0f}};
  const T3DVec3 camTarget = {{0,0,0}};

  uint8_t colorAmbient[4] = {80, 80, 110, 0xFF};
  uint8_t colorDir[4]     = {90, 80, 80, 0xFF};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  T3DModel *model = t3d_model_load("rom:/box.t3dm");
  T3DModel *target = t3d_model_load("rom:/target.t3dm");

  rspq_block_begin();
  t3d_matrix_set_mul(modelMatFP, 1, 0);
  t3d_model_draw(model);
  rspq_block_t *dplDraw = rspq_block_end();

  rspq_block_begin();
  t3d_matrix_set_mul(modelTargetMatFP, 1, 0);
  t3d_model_draw_custom(target, (T3DModelDrawConf){
    .userData = &offscreenSurf,
    .dynTextureCb = dynamic_tex_cb,
  });
  rspq_block_t *dplTarget = rspq_block_end();

  float rotAngle = -2.4f;

  for(;;)
  {
    // ======== Update ======== //
    rotAngle += 0.005f;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 200.0f);
    t3d_viewport_look_at(&viewport, &(T3DVec3){{0,10.0f,85.0f}}, &camTarget);

    t3d_viewport_set_projection(&viewportOffscreen, T3D_DEG_TO_RAD(85.0f), 10.0f, 400.0f);
    t3d_viewport_look_at(&viewportOffscreen, &camPos, &camTarget);

    t3d_mat4fp_from_srt_euler(modelMatFP,
      (float[3]){0.2f, 0.2f, 0.2f},
      (float[3]){rotAngle*1.3f, rotAngle*1.6f, rotAngle*1.0f},
      (float[3]){0,0,0}
    );

    t3d_mat4fp_from_srt_euler(modelTargetMatFP,
      (float[3]){0.1f, 0.1f, 0.1f},
      (float[3]){0.0f, rotAngle*0.5f, 0.0f},
      (float[3]){0,-7,0}
    );

    // ======== Draw (Offscreen) ======== //

    // Render the offscreen-scene first, for that we attach the extra buffer instead of the screen one
    rdpq_attach_clear(&offscreenSurf, &offscreenSurfZ);

    rdpq_set_mode_fill(RGBA32(0x33, 0x33, 0x99, 0xFF));
    rdpq_fill_rectangle(0, 0, OFFSCREEN_SIZE, OFFSCREEN_SIZE);

    // after that is done, the rendering part is exactly the same as before
    t3d_frame_start();
    t3d_viewport_attach(&viewportOffscreen);

    t3d_light_set_ambient((uint8_t[4]){0xFF, 0xFF, 0xFF, 0xFF});
    t3d_light_set_count(0);

    rspq_block_run(dplDraw);

    // we can also draw 2D elements as usual
    t3d_debug_print_start();
    t3d_debug_printf(8, 8, "%.1f FPS\n", display_get_fps());

    rdpq_detach_wait(); // to finish, detach and wait for the RDP to render the offscreen buffer

    // ======== Draw (Onscreen) ======== //
    // For our main scene we now attach the screen buffer again,
    // from now one everything is the same as in other examples
    rdpq_attach(display_get(), &depthBuffer);

    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(100, 80, 80, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    // the model uses the prim. color to blend between the offscreen-texture and white-noise
    uint8_t blendA = random_flicker(rotAngle * 6.5f) * 255.4f;
    uint8_t blendB = 255 - blendA;
    rdpq_set_prim_color(RGBA32(blendA, blendA, blendA, blendB));

    // this block here will render the model that uses the offscreen texture
    // at this point everything was already baked into the DPL, so no extra costs are involved for drawing it
    rspq_block_run(dplTarget);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

