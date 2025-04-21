#include <libdragon.h>
#include <rspq_profile.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include "debug_overlay.h"

/**
 * Example project showcasing HDR rendering.
 * This will both use a special ucode feature as well as a specific material setup.
 *
 * Tiny3D allows for an exposure setting, which will simply scale the final color (vertex+lighting)
 * before passing it to the RDP.
 * If color gets scaled above 1.0 it will be clamped by the ucode.
 * In contrast the RDP, where you can only add or scale down, this allows for higher amplifications of color.
 * Also avoiding the usual overflow in the process.
 * Once that clamped color reaches the RDP, it can be used in a special CC setup to simulate a bloom effect.
 *
 * Special thanks to https://github.com/SpookyIluha for providing this demo!
 */

surface_t* fb = NULL;

float exposure = 30.0f;
float exposure_bias = 0.7;
float average_brightness = 0;

// autoexposure function for HDR lighting, this will control how bright the vertex colors are in range of 0-1 after T&L for the RDP HDR modulation through color combiner
void exposure_set(void* framebuffer){
  if(!framebuffer) return;
  surface_t* frame = (surface_t*) framebuffer;

  // sample points across the screen
  color_t colors[5] = {0};
  uint16_t* pixels = frame->buffer; // get the previous framebuffer (assumed 320x240 RGBA16 format), it shouldn't be cleared for consistency

  colors[0] = color_from_packed16(pixels[(int)60 * frame->width + (int)70]);
  colors[1] = color_from_packed16(pixels[(int)60 * frame->width + (int)250]);
  colors[2] = color_from_packed16(pixels[(int)180 * frame->width + (int)70]);
  colors[3] = color_from_packed16(pixels[(int)180 * frame->width + (int)250]);
  colors[4] = color_from_packed16(pixels[(int)120 * frame->width + (int)160]);

  average_brightness = 0;
  for(int i = 0; i < 5; i++){
    average_brightness += colors[i].r;
    average_brightness += colors[i].g;
    average_brightness += colors[i].b;
  }

  average_brightness /= 3825.0f; // normalize the brightness to 0-1 scale (255*3*5 = 3825)

  // exposure bracket uses an overall bias of how the bright the framebuffer is at 0-1 scale
  // eg. if the avegare brightness of the framebuffer is > 0.7, then the exposure needs to go down until
  // the average brightness is 0.7, and the other way around
  if(average_brightness > exposure_bias) {exposure -= 0.05f;}
  else if(average_brightness < exposure_bias - 0.3f) {exposure += 0.05f;}

  // min/max exposure levels
  if(exposure > 10) exposure -= 0.25f;
  if(exposure < 0) exposure = 0;
}

[[noreturn]]
int main()
{
  profile_data.frame_count = 0;
	//debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);

  rdpq_init();
  rspq_profile_start();

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport[3];
  viewport[0] = t3d_viewport_create();
  viewport[1] = t3d_viewport_create();
  viewport[2] = t3d_viewport_create();

  t3d_debug_print_init();
  sprite_t *spriteLogo = sprite_load("rom:/logo.ia8.sprite");
  T3DModel *model = t3d_model_load("rom://arcvis_baked_282.t3dm");
  rdpq_font_t* font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
  rdpq_text_register_font(1, font);

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
  float modelScale = 0.15f;
  t3d_mat4fp_from_srt_euler(modelMatFP,
    (float[3]){modelScale, modelScale, modelScale},
    (float[3]){0,0,0},
    (float[3]){0,0,0}
  );

  rspq_block_begin();
    t3d_matrix_push(modelMatFP);
    t3d_model_draw(model);
    t3d_matrix_pop(1);
  model->userBlock = rspq_block_end();

  T3DVec3 camPos = {{0.0, 0.0, 0.0}};
  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};

  float camRotX = 3.14f;
  float camRotY = 0.24f;

  uint8_t colorAmbient[4] = {100, 100, 100, 0xFF};

  bool requestDisplayMetrics = false;
  bool displayMetrics = false;
  float last3dFPS = 0.0f;

  for(uint64_t frame = 0;; ++frame)
  {
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    float deltaTime = display_get_delta_time();

    {
      float camSpeed = deltaTime * 0.3f;
      float camRotSpeed = deltaTime * 0.01f;

      camDir.v[0] = fm_cosf(camRotX) * fm_cosf(camRotY);
      camDir.v[1] = fm_sinf(camRotY);
      camDir.v[2] = fm_sinf(camRotX) * fm_cosf(camRotY);
      t3d_vec3_norm(&camDir);

      if(joypad.btn.z) {
        camRotX += (float)joypad.stick_x * camRotSpeed;
        camRotY += (float)joypad.stick_y * camRotSpeed;
      } else {
        camPos.v[0] += camDir.v[0] * (float)joypad.stick_y * camSpeed;
        camPos.v[1] += camDir.v[1] * (float)joypad.stick_y * camSpeed;
        camPos.v[2] += camDir.v[2] * (float)joypad.stick_y * camSpeed;

        camPos.v[0] += camDir.v[2] * (float)joypad.stick_x * -camSpeed;
        camPos.v[2] -= camDir.v[0] * (float)joypad.stick_x * -camSpeed;
      }

      if(joypad.btn.c_up) exposure_bias += 0.05f;
      if(joypad.btn.c_down) exposure_bias -= 0.05f;
      if(exposure_bias > 1) exposure_bias = 1;
      if(exposure_bias < 0.3f) exposure_bias = 0.3f;

      camTarget.v[0] = camPos.v[0] + camDir.v[0];
      camTarget.v[1] = camPos.v[1] + camDir.v[1];
      camTarget.v[2] = camPos.v[2] + camDir.v[2];
    }

    if(joypad.btn.b)
    {
      requestDisplayMetrics = true;
    } else {
      requestDisplayMetrics = false;
      displayMetrics = false;
    }

    t3d_viewport_set_projection(&viewport[(frame + 1) % 3], T3D_DEG_TO_RAD(85.0f), 2.5f, 100.0f);
    t3d_viewport_look_at(&viewport[(frame + 1) % 3], &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // ----------- DRAW ------------ //
    exposure_set(fb);
    fb = display_get();
    rdpq_attach(fb, display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport[(frame) % 3]);
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(0);
    t3d_light_set_exposure(exposure);

    rdpq_sync_pipe();
    rspq_block_run(model->userBlock);
    rdpq_sync_pipe();

    if(displayMetrics)
    {
      if(profile_data.frame_count == 0) {
        rdpq_text_printf(NULL, 1, 140, 206, "FPS (3D)   : %.4f", last3dFPS);
        rdpq_text_printf(NULL, 1, 140, 218, "FPS (3D+UI): %.4f", display_get_fps());
      }

      debug_draw_perf_overlay(last3dFPS);

      rdpq_set_mode_standard();
      rdpq_mode_combiner(RDPQ_COMBINER_TEX);
      rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
      rdpq_sprite_blit(spriteLogo, 22.0f, 164.0f, NULL);
      rspq_wait();
    }
    else{
      rdpq_text_printf(NULL, 1, 16, 16, "Exposure: %.2f", exposure);
      rdpq_text_printf(NULL, 1, 16, 24, "Exp bias: %.2f", exposure_bias);
      rdpq_text_printf(NULL, 1, 16, 32, "Brightness avg: %.2f", average_brightness);
    }

    rdpq_detach_show();
    rspq_profile_next_frame();

    if(frame == 30)
    {
      if(!displayMetrics){
        last3dFPS = display_get_fps();
        rspq_wait();
        rspq_profile_get_data(&profile_data);
        if(requestDisplayMetrics)displayMetrics = true;
      }

      frame = 0;
      rspq_profile_reset();
    }
  }
}
