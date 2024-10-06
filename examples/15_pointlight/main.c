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
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

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
  T3DModel *modelLight = t3d_model_load("rom://light.t3dm");

/*T3DMaterial *celShadeMat = t3d_model_get_material(model, "MatColor");
if(celShadeMat) {
  celShadeMat->vertexFxFunc = T3D_VERTEX_FX_CELSHADE_ALPHA;
}*/

  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  t3d_mat4_identity(&modelMat);
  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP* matLightFP[5] = {
    malloc_uncached(sizeof(T3DMat4FP)),
    malloc_uncached(sizeof(T3DMat4FP)),
    malloc_uncached(sizeof(T3DMat4FP)),
    malloc_uncached(sizeof(T3DMat4FP)),
    malloc_uncached(sizeof(T3DMat4FP)),
  };

  T3DVec3 camPos = {{-2.9232f, 67.6248f, 61.1093f}};

  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,-1}};

  float camRotX = 1.544792654048f;
  float camRotY = 4.05f;

  T3DVec4 pointLights[5] = {
    {{ 150.0f, 20.0f, -5.0f,  0.055f}},
    {{-150.0f, 20.0f, -5.0f,  0.055f}},
    {{   0.0f,  70.0f, -180.0f, 0.1f}},
    {{0.0f,  20.0f, 30.0f, 0.055f}},
    {{0, 25.0f, 0.0f,  0.15f}},
  };
  color_t lightColors[5] = {
    {0xFF, 0xFF, 0x1F, 0xFF},
    {0x1F, 0xFF, 0x1F, 0xFF},
    {0x1F, 0x1F, 0xFF, 0xFF},
    {0xFF, 0x1F, 0x1F, 0xFF},
    {0xFF, 0x2F, 0x1F, 0xFF},
  };
  color_t lightColorOff = {0,0,0, 0xFF};

  uint32_t currLight = 0;

  uint8_t colorAmbient[4] = {20, 20, 20, 0xFF};
  uint8_t colorDir[4]     = {0xFF, 0xFF, 0xFF, 0xFF};

  T3DVec3 lightDirVec = {{0.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  float rotAngle = 0.0f;
  T3DVec3 rotAxis = {{1.0f, 2.5f, 0.25f}};
  t3d_vec3_norm(&rotAxis);

  rspq_block_t *dplDraw = NULL;
  rspq_block_t *dplLight = NULL;

  double lastTimeMs = 0;
  float time = 0.0f;

  bool requestDisplayMetrics = false;
  bool displayMetrics = false;
  float last3dFPS = 0.0f;

  bool isOrtho = false;
  float posXY[2] = {0,0};

  for(uint64_t frame = 0;; ++frame)
  {
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    if(btn.start)isOrtho = !isOrtho;

    double nowMs = (double)get_ticks_us() / 1000.0;
    float deltaTime = (float)(nowMs - lastTimeMs);
    lastTimeMs = nowMs;
    time += deltaTime;


    if(btn.l)currLight--;
    if(btn.r)currLight++;
    currLight = currLight % 5;
    T3DVec4 *lightPos = &pointLights[currLight];


    {
      float camSpeed = deltaTime * 0.001f;
      float camRotSpeed = deltaTime * 0.00001f;

      camDir.v[0] = fm_cosf(camRotX) * fm_cosf(camRotY);
      camDir.v[1] = fm_sinf(camRotY);
      camDir.v[2] = fm_sinf(camRotX) * fm_cosf(camRotY);
      t3d_vec3_norm(&camDir);


      if(joypad.btn.z) {
        if(joypad.btn.a) {
          lightPos->v[3] += (float)joypad.stick_y * camSpeed * 0.025f;
          lightPos->v[3] = fmaxf(0.0f, lightPos->v[3]);
          lightPos->v[3] = fminf(1.0f, lightPos->v[3]);
        } else {
          lightPos->v[0] += camDir.v[2] * (float)joypad.stick_x * -camSpeed;
          lightPos->v[2] -= camDir.v[0] * (float)joypad.stick_x * -camSpeed;

          lightPos->v[0] += camDir.v[0] * (float)joypad.stick_y * camSpeed;
          lightPos->v[2] += camDir.v[2] * (float)joypad.stick_y * camSpeed;

          lightPos->v[1] += (float)joypad.btn.c_up * camSpeed * 45.0f;
          lightPos->v[1] -= (float)joypad.btn.c_down * camSpeed * 45.0f;
        }


      } else {
        if(joypad.btn.a) {
          camRotX += (float)joypad.stick_x * camRotSpeed;
          camRotY += (float)joypad.stick_y * camRotSpeed;
        } else {
          camPos.v[0] += camDir.v[0] * (float)joypad.stick_y * camSpeed;
          camPos.v[1] += camDir.v[1] * (float)joypad.stick_y * camSpeed;
          camPos.v[2] += camDir.v[2] * (float)joypad.stick_y * camSpeed;

          camPos.v[0] += camDir.v[2] * (float)joypad.stick_x * -camSpeed;
          camPos.v[2] -= camDir.v[0] * (float)joypad.stick_x * -camSpeed;
        }

        if(isOrtho) {
          if(joypad.btn.c_up)posXY[1] += camSpeed * 85.0f;
          if(joypad.btn.c_down)posXY[1] -= camSpeed * 85.0f;
          if(joypad.btn.c_left)posXY[0] -= camSpeed * 85.0f;
          if(joypad.btn.c_right)posXY[0] += camSpeed * 85.0f;
        } else {
          if(joypad.btn.c_up)camPos.v[1] += camSpeed * 85.0f;
          if(joypad.btn.c_down)camPos.v[1] -= camSpeed * 85.0f;
        }
      }

      camTarget.v[0] = camPos.v[0] + camDir.v[0];
      camTarget.v[1] = camPos.v[1] + camDir.v[1];
      camTarget.v[2] = camPos.v[2] + camDir.v[2];
    }


    requestDisplayMetrics = joypad.btn.b;
    if(!joypad.btn.b)displayMetrics = false;

    rotAngle += 0.03f;

    float modelScale = 0.15f;
    t3d_mat4_identity(&modelMat);
    //t3d_mat4_rotate(&modelMat, &rotAxis, rotAngle);
    t3d_mat4_scale(&modelMat, modelScale, modelScale, modelScale);
    t3d_mat4_to_fixed(modelMatFP, &modelMat);

    for(int i=0; i<5; ++i)
    {
      float scale = i == currLight ? 0.025f : 0.015f;
      if(i == 4)scale *= 6;
      t3d_mat4fp_from_srt_euler(matLightFP[i],
        (float[]){scale, scale, scale},
        (float[]){ fm_cosf(time * 0.002f), fm_sinf(time * 0.002f), fm_cosf(time * 0.002f)},
        (float[]){pointLights[i].v[0], pointLights[i].v[1], pointLights[i].v[2]}
      );
    }

    if(isOrtho) {
      t3d_viewport_set_ortho(&viewport, posXY[0], posXY[0]+320/8*7, posXY[1], posXY[1]+240/8*7, 3.0f, 300.0f);
    } else {
      t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(70.0f), 3.0f, 220.0f);
    }
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // ----------- DRAW ------------ //
    rdpq_attach(display_get(), display_get_zbuf());

    t3d_frame_start();
    rdpq_mode_antialias(isOrtho ? AA_NONE : AA_STANDARD);
    rdpq_mode_dithering(DITHER_SQUARE_SQUARE);

    t3d_viewport_attach(&viewport);

    rdpq_set_prim_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});

    t3d_screen_clear_color(RGBA32(0, 0, 0, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient); // one global ambient light, always active
    t3d_light_set_directional(0, colorDir, &lightDirVec); // optional directional light, can be disabled

    pointLights[4].v[1] = 23.0f + (sinf(time * 0.001f) * 4.0f);
    pointLights[4].v[3] = 0.2f
       + (fm_sinf(time * 0.005f) * 0.015f)
       + ((fm_cosf(time * 0.006f)+1) * 0.0025f)
       + ((fm_sinf(time * 0.002f)+0.9f) * 0.01f)
       + ((fm_sinf(time * 0.001f)+0.8f) * 0.01f);


    for(int i=0; i<5; ++i) {
      t3d_light_set_point(i, &lightColors[i].r, (T3DVec3*)&pointLights[i], pointLights[i].v[3]);
    }
    t3d_light_set_count(5);

    // t3d functions can be recorded into a display list:
    if(!dplDraw) {
      rspq_block_begin();
      t3d_model_draw(model);
      dplDraw = rspq_block_end();
    }

    if(!dplLight) {
      rspq_block_begin();
      t3d_model_draw(modelLight);
      dplLight = rspq_block_end();
    }

    t3d_matrix_push_pos(1);
    t3d_matrix_set(modelMatFP, true);
    rspq_block_run(dplDraw);

    for(int i=0; i<5; ++i) {
      rdpq_set_prim_color(pointLights[i].v[3] <= 0.0f ? lightColorOff : lightColors[i]);
      t3d_matrix_set(matLightFP[i], true);
      rspq_block_run(dplLight);
    }
    t3d_matrix_pop(1);


    t3d_debug_print_start();
    t3d_debug_printf(220, 218, "FPS: %.2f", display_get_fps());
    //t3d_debug_printf(16, 16, "Size: %.4f", pointLights[currLight].v[3]);

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
