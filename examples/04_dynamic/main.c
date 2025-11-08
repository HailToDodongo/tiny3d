#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

#define FB_COUNT 3

/**
 * Showcase for dynamically transformed meshes.
 * This still uses GLTF file, but modifies the data at runtime.
 */

// Hook/callback to modify tile settings set by t3d_model_draw
void tile_scroll(void* userData, rdpq_texparms_t *tileParams, rdpq_tile_t tile) {
  float offset = *(float*)userData;
  if(tile == TILE0) {
    tileParams->s.translate = offset * 0.5f;
    tileParams->t.translate = offset * 0.8f;

    tileParams->s.translate = fm_fmodf(tileParams->s.translate, 32.0f);
    tileParams->t.translate = fm_fmodf(tileParams->t.translate, 32.0f);
  }
}

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, FB_COUNT, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  joypad_init();

  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create_buffered(FB_COUNT);
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP)); // no need to buffer this matrix, since it never updates
  t3d_mat4fp_from_srt_euler(modelMatFP,
    (float[3]){0.12f, 0.12f, 0.12f},
    (float[3]){0,0,0},
    (float[3]){0,0,0}
  );

  const T3DVec3 camTarget = {{0,0,0}};
  float camAngle = 0.0f;

  uint8_t colorAmbient[4] = {0xF0, 0xF0, 0xF0, 0xFF};

  T3DModel *modelRoom = t3d_model_load("rom:/model.t3dm");
  T3DModel *modelLava = t3d_model_load("rom:/lava.t3dm");

  // returns the global vertex buffer for a model.
  // If you have multiple models and want to only update one, you have to manually iterate over the objects.
  // see the implementation of t3d_model_draw_custom in that case.
  T3DVertPacked* vertsLava = t3d_model_get_vertices(modelLava);
  uint32_t byteSizeVertices = sizeof(T3DVertPacked) * modelLava->totalVertCount / 2;

  // since the RSP runs in parallel to the CPU, we want to multi-buffer the vertices here.
  // This avoids the case where the RSP may read values partially overwritten by the CPU for the next frame.
  // So allocate 2 extra copies of the vertices here...
  T3DVertPacked* vertsBuff[3] = {vertsLava, malloc(byteSizeVertices), malloc(byteSizeVertices)};
  // ...and fill the other buffers with the initial data
  memcpy(vertsBuff[1], vertsLava, byteSizeVertices);
  memcpy(vertsBuff[2], vertsLava, byteSizeVertices);

  // the static room can be drawn normally
  rspq_block_begin();
    t3d_model_draw(modelRoom);
  rspq_block_t *dplRoom = rspq_block_end();

  // for the lava we want to grab the lava object...
  auto objLava = t3d_model_get_object_by_index(modelLava, 0);
  // ... and replace the actual vertex address with a placeholder (see 11_segments example for more on that)
  // this means the address to vertices is replaced with a relative offset internally,
  // and we need to set the absolute starting address later on before drawing.
  t3d_model_make_object_vert_placeholder(modelLava, objLava, 1);

  // after that we can still fully record the model as usual
  rspq_block_begin();
    t3d_model_draw(modelLava);
  rspq_block_t *dplLava = rspq_block_end();

  bool scrollEnabled = true;
  bool transformEnabled = true;

  float tileOffset = 0.0f;
  float transformOffset = 0.0f;
  int frameIdx = 0;

  for(;;)
  {
    // ======== Update ======== //
    frameIdx = (frameIdx + 1) % 3;
    joypad_poll();
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(btn.a)scrollEnabled = !scrollEnabled;
    if(btn.b)transformEnabled = !transformEnabled;

    camAngle -= 0.005f;
    tileOffset += 0.1f;

    T3DVec3 camPos = {{sinf(camAngle) * 45.0f, 35.0f, cosf(camAngle) * 60.0f}};

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // transform mesh
    if(transformEnabled)
    {
      transformOffset += 0.0042f;
      float globalHeight = fm_sinf(transformOffset * 2.5f) * 30.0f;

      // grab the next vertex buffer to modify, 'frameIdx' here cycles through the 3 buffers each frame.
      // So while we modify 'frameIdx', the RSP can still safely draw 'frameIdx-1' and 'frameIdx-2'.
      T3DVertPacked* verts = vertsBuff[frameIdx];

      for(uint16_t i=0; i < modelLava->totalVertCount; ++i)
      {
        // To better handle the interleaved vertex format,
        // t3d provides a few helper functions to access attributes
        int16_t *pos = t3d_vertbuffer_get_pos(verts, i);

        // water-like wobble effect
        float height = fm_sinf(
          transformOffset * 4.5f
          + pos[0] * 30.1f
          + pos[2] * 20.1f
        );
        pos[1] = 20.0f * height + globalHeight;

        // make lower parts darker, and higher parts brighter
        float color = height * 0.25f + 0.75f;
        uint8_t* rgba = t3d_vertbuffer_get_rgba(verts, i);
        rgba[0] = color * 255;
        rgba[1] = color * 200;
        rgba[2] = color * 200;
        rgba[3] = 0xFF;
      }

      // Don't forget to flush the cache again! (or use an uncached buffer in the first place)
      data_cache_hit_writeback(verts, sizeof(T3DVertPacked) * modelLava->totalVertCount / 2);
    }

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color((color_t){140, 50, 20, 0xFF});

    t3d_screen_clear_color(RGBA32(111, 20, 20, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_count(0);

    t3d_matrix_push(modelMatFP);

    t3d_light_set_ambient((uint8_t[]){0xFF, 0xFF, 0xFF, 0xFF});

    // Draw lava, now we can set which vertex buffer to use.
    // The segment (first arg) must be the same as the one chosen in t3d_model_make_object_placeholder above.
    // Note that this id is global, so if you have multiple dynamic objects make sure to keep it unique,
    // or set it between draws.
    // Other draws (like the static room) that don't use placeholders are not affected by this.
    t3d_segment_set(1, vertsBuff[frameIdx]);

    /**
     * To draw a dynamic mesh you can use a recorded block.
     * If you want to modify tile-settings however you have to use the custom draw function.
     * Since tile settings are baked into the display list, you can't change them afterwards.
     */
    if(scrollEnabled) {
      t3d_model_draw_custom(modelLava, (T3DModelDrawConf){
        .userData = &tileOffset,
        .tileCb = tile_scroll,
      });
    } else {
      rspq_block_run(dplLava);
    }

    // Draw room:
    t3d_fog_set_range(0.4f, 80.0f);
    t3d_fog_set_enabled(true);

    t3d_light_set_ambient(colorAmbient);
    rspq_block_run(dplRoom);

    t3d_matrix_pop(1);

    // ======== Draw (2D) ======== //
    rdpq_sync_pipe();
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16.0f, 20.0f, "[A] Scroll: %c\n", scrollEnabled ? 'Y' : '-');
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16.0f, 32.0f, "[B] Transf: %c\n", transformEnabled ? 'Y' : '-');
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 220.0f, 20.0f, "FPS: %.4f\n", display_get_fps());

    rdpq_detach_show();
  }

  t3d_viewport_destroy(&viewport);
  t3d_destroy();
  return 0;
}

