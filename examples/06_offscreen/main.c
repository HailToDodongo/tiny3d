#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

/**
 * Showcase for offscreen rendering.
 * This renders a scene & video into a texture which then can be used by 3D models.
 */
#define OFFSCREEN_SIZE 80

// This is a callback for t3d_model_draw_custom, it is used when a texture in a model is set to dynamic/"reference"
void dynamic_tex_cb(void* userData, const T3DMaterial* material, rdpq_texparms_t *tileParams, rdpq_tile_t tile) {
  if(tile != TILE0)return; // this callback can happen 2 times per mesh, you are allowed to skip calls

  surface_t *offscreenSurf = (surface_t*)userData;
  rdpq_sync_tile();

  int sHalf = OFFSCREEN_SIZE / 2;
  int sFull = OFFSCREEN_SIZE;

  // upload a slice of the offscreen-buffer, the screen in the TV model is split into 4 materials for each section
  // if you are working with a small enough single texture, you can ofc use a normal sprite upload.
  // the quadrant is determined by the texture reference set in fast64, which can be used as an arbitrary value
  switch(material->textureA.texReference) { // Note: TILE1 is used here due to CC shenanigans
    case 1: rdpq_tex_upload_sub(TILE1, offscreenSurf, NULL,  0,     0,     sHalf, sHalf); break;
    case 2: rdpq_tex_upload_sub(TILE1, offscreenSurf, NULL,  sHalf, 0,     sFull, sHalf); break;
    case 3: rdpq_tex_upload_sub(TILE1, offscreenSurf, NULL,  0,     sHalf, sHalf, sFull); break;
    case 4: rdpq_tex_upload_sub(TILE1, offscreenSurf, NULL,  sHalf, sHalf, sFull, sFull); break;
  }
}

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  // Allocate our normal screen and depth buffer...
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  //... then a smaller additional buffer (color + depth) for offscreen rendering
  surface_t offscreenSurf = surface_alloc(FMT_RGBA16, OFFSCREEN_SIZE, OFFSCREEN_SIZE);
  surface_t offscreenSurfZ = surface_alloc(FMT_RGBA16, OFFSCREEN_SIZE, OFFSCREEN_SIZE);

  rdpq_init();
  joypad_init();
  yuv_init();

  t3d_init((T3DInitParams){});
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  // Like in the demo before we can create multiple viewports.
  // This time however we have to match the offscreen one to the size of the offscreen buffer
  T3DViewport viewport = t3d_viewport_create();
  T3DViewport viewportOffscreen = t3d_viewport_create();
  t3d_viewport_set_area(&viewportOffscreen, 0, 0, OFFSCREEN_SIZE, OFFSCREEN_SIZE);

  T3DMat4FP* matrixBox = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP* matrixCRT = malloc_uncached(sizeof(T3DMat4FP));

  t3d_mat4fp_from_srt_euler(matrixCRT,
    (float[3]){0.02f, 0.02f, 0.02f},
    (float[3]){0,0,0}, (float[3]){0,-1,0}
  );

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  T3DModel *modelBox = t3d_model_load("rom:/box.t3dm");
  T3DModel *modelCRT = t3d_model_load("rom:/target.t3dm");

  mpeg2_t *mp2 = mpeg2_open("rom:/video.m1v");
  yuv_blitter_t yuvBlitter = yuv_blitter_new_fmv(
    mpeg2_get_width(mp2), mpeg2_get_height(mp2),
    OFFSCREEN_SIZE, OFFSCREEN_SIZE, NULL);

  rspq_block_begin();
  t3d_matrix_push(matrixBox);
  t3d_model_draw(modelBox);
  t3d_matrix_pop(1);
  rspq_block_t *dplBox = rspq_block_end();

  rspq_block_begin();
  t3d_matrix_push(matrixCRT);
  t3d_model_draw_custom(modelCRT, (T3DModelDrawConf){
    .userData = &offscreenSurf,
    .dynTextureCb = dynamic_tex_cb,
  });
  t3d_matrix_pop(1);
  rspq_block_t *dplCRT = rspq_block_end();

  float rotAngle = 2.4f;
  float noiseStrength = 1.0f;
  float camDist = 20.0f;
  float lastTime = 0.0f;
  float videoFrameTime = 1.0f;
  bool offscreen3D = false;

  for(;;)
  {
    // ======== Update ======== //
    float timeMs = (float)((double)get_ticks_us() / 1000.0);
    float deltaTime = (timeMs - lastTime) / 1000.0f;
    lastTime = timeMs;
    videoFrameTime += deltaTime;

    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);

    if(joypad_get_buttons_pressed(JOYPAD_PORT_1).a)offscreen3D = !offscreen3D;

    // camera rotation (+ noise falloff)
    float manualRot = ((float)joypad.stick_x) * deltaTime * 0.7f;
    rotAngle += (deltaTime*-0.3f) + (manualRot * -0.05f);
    noiseStrength = fminf(fmaxf(noiseStrength, fabsf(manualRot)), 1.0f) * 0.96f;

    // zoom-in / out
    camDist = fmaxf(14.0f, fminf(30.0f, camDist + (float)joypad.stick_y * -deltaTime * 0.6f));
    T3DVec3 camPos = {{sinf(rotAngle) * camDist, 1.5f, cosf(rotAngle) * camDist}};

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 1.5f, 100.0f);
    t3d_viewport_look_at(&viewport, &camPos, &(T3DVec3){{0,0,0}}, &(T3DVec3){{0,1,0}});

    t3d_viewport_set_projection(&viewportOffscreen, T3D_DEG_TO_RAD(85.0f), 5.0f, 150.0f);
    t3d_viewport_look_at(&viewportOffscreen, &(T3DVec3){{0,5.0f,40.0f}}, &(T3DVec3){{0,0,0}}, &(T3DVec3){{0,1,0}});

    t3d_mat4fp_from_srt_euler(matrixBox,
      (float[3]){0.2f, 0.2f, 0.2f},
      (float[3]){rotAngle*1.3f, rotAngle*1.6f, rotAngle*1.0f},
      (float[3]){0,0,0}
    );

    // ======== Draw (Offscreen) ======== //
    // Render the offscreen-scene first, for that we attach the extra buffer instead of the screen one
    rdpq_attach_clear(&offscreenSurf, &offscreenSurfZ);

    // while it is a bit outside the scope of t3d, we can draw other things like videos into buffers too.
    // For more information on how to use the mpeg2 library, see the "videoplayer" example in libdragon
    if(videoFrameTime > 0.015f) {
      videoFrameTime = 0.0f;
      if(!mpeg2_next_frame(mp2)) {
        mpeg2_rewind(mp2);
        mpeg2_next_frame(mp2);
      }
    }
    yuv_frame_t frame = mpeg2_get_frame(mp2);
    yuv_blitter_run(&yuvBlitter, &frame);

    // the 3D rendering part itself is exactly the same as before
    // just attach the offscreen-viewport instead and draw the scene
    if(offscreen3D) {
      t3d_frame_start();
      t3d_viewport_attach(&viewportOffscreen);

      t3d_light_set_ambient((uint8_t[4]){0xFF, 0xFF, 0xFF, 0xFF});
      t3d_light_set_count(0);

      rspq_block_run(dplBox);

      rdpq_sync_pipe();
      rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 14, "%.1f FPS\n", display_get_fps());
    }

    rdpq_detach(); // to finish, simply detach (no explicit wait is needed)

    // ======== Draw (Onscreen) ======== //
    // For our main scene we now attach the screen buffer again,
    // from now one everything is the same as in other examples
    rdpq_attach(display_get(), display_get_zbuf());

    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(170, 140, 140, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient((uint8_t[4]){180, 150, 150, 0xFF});
    t3d_light_set_directional(0, (uint8_t[4]){100, 100, 120, 0xFF}, &lightDirVec);
    t3d_light_set_count(1);

    // the model uses the prim. color to blend between the offscreen-texture and white-noise
    uint8_t blend = (uint8_t)(noiseStrength * 255.4f);
    rdpq_set_prim_color(RGBA32(blend, blend, blend, 255 - blend));

    // this block here will render the model that uses the offscreen texture
    // at this point everything was already baked into the DPL, so no extra costs are involved for drawing it
    rspq_block_run(dplCRT);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}
