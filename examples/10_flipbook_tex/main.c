#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

/**
 * Example project showcasing "flip-book" textures, aka. animated textures.
 * This uses a static model which includes both normal and dynamic textures.
 */

/**
 * This is called once during the `t3d_model_draw_custom` call when a placeholder is encountered.
 * You are free to add any logic here, in this case we simply upload a surface directly.
 * See the example `06_offscreen` for a more complex example.
 */
void dynamic_tex_cb(void* userData, const T3DMaterial* material, rdpq_texparms_t *tileParams, rdpq_tile_t tile) {
  if(tile != TILE0)return; // we only want to set one texture
  // 'userData' is a value you can pass into 't3d_model_draw_custom', this can be any value or struct you want...
  surface_t* surface = (surface_t*)userData; // ...in this case it is a surface pointer

  rdpq_sync_tile();
  rdpq_mode_tlut(TLUT_NONE);
  rdpq_tex_upload(TILE0, surface, NULL);
}

float get_time_s() {
  return (float)((double)get_ticks_us() / 1000000.0);
}

int main()
{
  debug_init_isviewer();
  debug_init_usblog();
  asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  //rdpq_debug_start();

  // Create a placeholder texture, this is used during recording of the model.
  // This allows us to only record the model once, and then change the texture later on.
  surface_t placeholder = surface_make_placeholder_linear(1, FMT_RGBA16, 32, 32);

  // Now load the actual textures, those are later set to be used to fill placeholders
  sprite_t* spritesWarn[2] = {
    sprite_load("rom:/warn00.rgba16.sprite"),
    sprite_load("rom:/warn01.rgba16.sprite"),
  };

  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();

  T3DMat4FP* mapMatFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DVec3 camPos    = {{-84, 12, 0}};
  T3DVec3 camTarget = {{-82, 12,-10}};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  uint8_t colorAmbient[4] = {0, 0, 100, 0x4F};
  color_t fogColor;

  T3DModel *modelMap = t3d_model_load("rom:/map.t3dm");

  rspq_block_begin();
    t3d_matrix_push(mapMatFP);
    // Draw the model with our callback now, this will embed the placeholder into it
    // (Remember to use the "Use Texture Reference" settings in fast64 to mark textures as dynamic)
    t3d_model_draw_custom(modelMap, (T3DModelDrawConf){
      .userData = &placeholder, // custom data you want to have in the callback (optional)
      .dynTextureCb = dynamic_tex_cb, // your callback (this pointer is only used for this call and is not saved)
      //.matrices = skeleton->boneMatricesFP // if this is a skinned mesh, set the bone matrices here too
    });
    t3d_matrix_pop(1);
  rspq_block_t *dplMap = rspq_block_end();

  float lastTime = get_time_s() - (1.0f / 60.0f);
  rspq_syncpoint_t syncPoint = 0;
  float rotY = 0.0f;

  for(;;)
  {
    // ======== Update ======== //
    float newTime = get_time_s();
    float deltaTime = newTime - lastTime;
    lastTime = newTime;

    rotY += deltaTime * -0.25f; // loop stage rotation
    if(rotY < -T3D_PI/4)rotY = 0;

    // blink timer for the demo, this controls the color and image index
    float blinkTimer = fm_fmodf(newTime*3, T3D_PI*4);
    uint8_t warnBlend = (fm_sinf(blinkTimer) * 0.5f + 0.5f) * 255;
    uint8_t fogIntens = warnBlend / 14;
    int textureIndex = (blinkTimer > (T3D_PI*1.5) && blinkTimer < (T3D_PI*3.5)) ? 0 : 1;

    if(textureIndex == 0) {
      fogColor = (color_t){fogIntens, fogIntens, 0x00, 0xFF};
      colorAmbient[0] = colorAmbient[2] + warnBlend / 4;
      colorAmbient[1] = colorAmbient[2] + warnBlend / 4;
    } else {
      fogColor = (color_t){fogIntens, 0x00, 0x00, 0xFF};
      colorAmbient[0] = colorAmbient[2] + warnBlend / 4;
      colorAmbient[1] = colorAmbient[2];
    }

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    if(syncPoint)rspq_syncpoint_wait(syncPoint); // wait for the RSP to process the previous frame

    t3d_mat4fp_from_srt_euler(mapMatFP,
      (float[3]){0.2f, 0.2f, 0.2f},
      (float[3]){0, rotY, 0},
      (float[3]){0, 0, 0}
    );

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_depth();

    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color(fogColor);
    rdpq_set_prim_color((color_t){warnBlend, warnBlend, warnBlend, 255});

    t3d_fog_set_range(-20.0f, 50.0f);
    t3d_fog_set_enabled(true);

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(0);

    // before drawing the recorded model, set the texture placeholders to the actual textures...
    rdpq_set_lookup_address(1, spritesWarn[textureIndex]->data);
    rspq_block_run(dplMap); // ...and then draw as usual

    syncPoint = rspq_syncpoint_new();
    rdpq_detach_show();
  }
  return 0;
}

