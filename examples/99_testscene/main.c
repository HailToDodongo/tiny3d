#include <libdragon.h>
#include <rspq_profile.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

#include "debug_overlay.h"

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

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  rspq_profile_start();
  //rdpq_debug_start();
  //rdpq_debug_log(true);

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();
  //viewport.guardBandScale = 1;

  t3d_debug_print_init();
  sprite_t *spriteLogo = sprite_load("rom:/logo.ia8.sprite");
  T3DModel *model = t3d_model_load("rom://scene.t3dm");

  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  t3d_mat4_identity(&modelMat);
  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DVec3 camPos = {{2.9232f, 67.6248f, 61.1093f}};
  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};

  float camRotX = 1.544792654048f;
  float camRotY = 4.05f;

  uint8_t colorAmbient[4] = {190, 190, 150, 0xFF};
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
      float camSpeed = deltaTime * 0.001f;
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

      if(joypad.btn.c_up)camPos.v[1] += camSpeed * 15.0f;
      if(joypad.btn.c_down)camPos.v[1] -= camSpeed * 15.0f;

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

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 2.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // ----------- DRAW ------------ //
    rdpq_attach(display_get(), display_get_zbuf());

    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    rdpq_set_prim_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color(fogColor);

    t3d_screen_clear_color(RGBA32(0, 0, 0, 0xFF));
    t3d_screen_clear_depth();

    t3d_fog_set_range(17.0f, 100.0f);
    t3d_fog_set_enabled(true);

    t3d_light_set_ambient(colorAmbient); // one global ambient light, always active
    t3d_light_set_directional(0, colorDir, &lightDirVec); // optional directional light, can be disabled
    t3d_light_set_count(1);

    /*t3d_light_set_point(0, colorDir, &(T3DVec3){{-10.0f, 10.0f, 0.0f}}, 0.3f, false); // optional point light, can be disabled
    t3d_light_set_point(1, colorDir, &(T3DVec3){{10.0f, 10.0f, 0.0f}}, 0.3f, false); // optional point light, can be disabled
    t3d_light_set_count(2);*/

    // t3d functions can be recorded into a display list:
    if(!model->userBlock) {
      rspq_block_begin();
      t3d_model_draw(model);
      model->userBlock = rspq_block_end();
    }

    t3d_matrix_push(modelMatFP);
    rspq_block_run(model->userBlock);
    t3d_matrix_pop(1);

    if(vertFxTime > 0.0f) {
      t3d_debug_print_start();
      t3d_debug_printf(16, 16, "VertexFX: %ld", vertFxFunc);
    }

    if(displayMetrics)
    {
      t3d_debug_print_start();

      // show pos / rot
      //debug_printf_screen(24, 190, "Pos: %.4f %.4f %.4f", camPos.v[0], camPos.v[1], camPos.v[2]);
      //t3d_debug_printf(24, 200, "Rot: %.4f %.4f", camRotX, camRotY);

      if(profile_data.frame_count == 0) {
        t3d_debug_printf(140, 206, "FPS (3D)   : %.4f", last3dFPS);
        t3d_debug_printf(140, 218, "FPS (3D+UI): %.4f", display_get_fps());
      }

      debug_draw_perf_overlay(last3dFPS);

      rdpq_set_mode_standard();
      rdpq_mode_combiner(RDPQ_COMBINER_TEX);
      rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
      rdpq_sprite_blit(spriteLogo, 22.0f, 164.0f, NULL);
      rspq_wait();
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
