#include <libdragon.h>
#include <rspq_profile.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

#include "debug_overlay.h"

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


/**
 * Simple example with a spinning quad.
 * This shows how to manually generate geometry and draw it,
 * although most o the time you should use the builtin model format.
 */
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
  //rdpq_debug_start();
  //rdpq_debug_log(true);

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport[3];
  viewport[0] = t3d_viewport_create();
  viewport[1] = t3d_viewport_create();
  viewport[2] = t3d_viewport_create();
  //viewport.guardBandScale = 1;

  t3d_debug_print_init();
  sprite_t *spriteLogo = sprite_load("rom:/logo.ia8.sprite");
  T3DModel *model = t3d_model_load("rom://arcvis_baked_282.t3dm");
  T3DModelState matstate = t3d_model_state_create();
  rdpq_font_t* font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
  rdpq_text_register_font(1, font);

  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  t3d_mat4_identity(&modelMat);
  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DVec3 camPos = {{0.0, 0.0, 0.0}};
  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};

  float camRotX = 3.14f;
  float camRotY = 0.24f;

  uint8_t colorAmbient[4] = {50, 50, 50, 0xFF};
  //uint8_t colorAmbient[4] = {40, 40, 40, 0xFF};
  uint8_t colorDir[4]     = {0xFF, 0xFF, 0xFF, 0xFF};

  T3DVec3 lightDirVec = {{0.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  float rotAngle = 0.0f;
  T3DVec3 rotAxis = {{1.0f, 2.5f, 0.25f}};
  t3d_vec3_norm(&rotAxis);

  double lastTimeMs = 0;
  float time = 0.0f;

  bool requestDisplayMetrics = false;
  bool displayMetrics = false;
  float last3dFPS = 0.0f;

  float vertFxTime = 0;
  int vertFxFunc = T3D_VERTEX_FX_NONE;
  for(uint64_t frame = 0;; ++frame)
  {
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    double nowMs = (double)get_ticks_us() / 1000.0;
    float deltaTime = (float)(nowMs - lastTimeMs);
    lastTimeMs = nowMs;
    time += deltaTime;

    vertFxTime = fmaxf(vertFxTime - deltaTime, 0.0f);

    {
      float camSpeed = deltaTime * 0.0003f;
      float camRotSpeed = deltaTime * 0.00001f;

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

    color_t fogColor = (color_t){0xFF, 0xFF, 0xFF, 0xFF};

    if(joypad.btn.b)
    {
      requestDisplayMetrics = true;
      colorAmbient[2] = colorAmbient[1] = colorAmbient[0] = 40;
      colorDir[2] = colorDir[1] = colorDir[0] = 40;
      fogColor = (color_t){0x00, 0x00, 0x00, 0xFF};
    } else {
      requestDisplayMetrics = false;
      displayMetrics = false;

      colorAmbient[2] = colorAmbient[1] = colorAmbient[0] = 100;
      colorDir[2] = colorDir[1] = colorDir[0] = 0xFF;

      // roate light around axis
      lightDirVec.v[0] = fm_cosf(time * 0.002f);
      lightDirVec.v[1] = 0.0f;//fm_sinf(time * 0.002f);
      lightDirVec.v[2] = fm_sinf(time * 0.002f);
      t3d_vec3_norm(&lightDirVec);
    }

    if(btn.l) {
      vertFxTime = 500.0f;
      vertFxFunc++;
      if(vertFxFunc > T3D_VERTEX_FX_OUTLINE)vertFxFunc = T3D_VERTEX_FX_NONE;
      t3d_state_set_vertex_fx(vertFxFunc, 32, 32);
    }

    rotAngle += 0.03f;

    float modelScale = 0.15f;
    t3d_mat4_identity(&modelMat);
    //t3d_mat4_rotate(&modelMat, &rotAxis, rotAngle);
    t3d_mat4_scale(&modelMat, modelScale, modelScale, modelScale);
    t3d_mat4_to_fixed(modelMatFP, &modelMat);

    t3d_viewport_set_projection(&viewport[(frame + 1) % 3], T3D_DEG_TO_RAD(85.0f), 2.5f, 100.0f);
    t3d_viewport_look_at(&viewport[(frame + 1) % 3], &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // ----------- DRAW ------------ //
    exposure_set(fb);
    fb = display_get();
    rdpq_attach(fb, display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport[(frame) % 3]);
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient); // one global ambient light, always active, we have baked vertex colors in this scene
    t3d_light_set_count(0);
    t3d_light_set_exposure(exposure);

    // t3d functions can be recorded into a display list:
    if(!model->userBlock) {
        rspq_block_begin();
        t3d_model_draw(model);
        model->userBlock = rspq_block_end();
    }
    rdpq_sync_pipe();
    t3d_matrix_push(modelMatFP);
    rspq_block_run(model->userBlock);
    t3d_matrix_pop(1);
    rdpq_sync_pipe();

    if(vertFxTime > 0.0f) {
      rdpq_text_printf(NULL, 1, 16, 16, "VertexFX: %i", vertFxFunc);
    }

    if(displayMetrics)
    {

      // show pos / rot
      //debug_printf_screen(24, 190, "Pos: %.4f %.4f %.4f", camPos.v[0], camPos.v[1], camPos.v[2]);
      //t3d_debug_printf(24, 200, "Rot: %.4f %.4f", camRotX, camRotY);

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

  t3d_destroy();
  return 0;
}
