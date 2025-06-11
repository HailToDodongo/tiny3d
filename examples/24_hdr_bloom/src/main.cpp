/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include "main.h"

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

#include "debugMenu.h"
#include "postProcess.h"
#include "render/debugDraw.h"
#include "rsp/rspFX.h"

namespace {
  constexpr int BUFF_COUNT = 3;

  State state{
    .ppConf = {
      .blurSteps = 5,
      .blurBrightness = 1.0f,
      .hdrFactor = 1.5f,
      .bloomThreshold = 0.2f,
     },
    .showOffscreen = false,
  };

  T3DVec3 camPos = {{-35.0, 21.0, 40.0}};
  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};
}

surface_t* fb = NULL;

[[noreturn]]
int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	//asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, BUFF_COUNT, GAMMA_NONE, FILTERS_RESAMPLE);

  rdpq_init();

  RspFX::init();
  PostProcess postProc[BUFF_COUNT]{};
  Debug::init();

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport[BUFF_COUNT];
  for(int i = 0; i < BUFF_COUNT; ++i) {
    viewport[i] = t3d_viewport_create();
  }

  T3DModel *model = t3d_model_load("rom://scene.t3dm");
  auto gradTest = sprite_load("rom:/gradTest.rgba16.sprite");

  T3DMat4FP* modelMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

  float modelScale = 0.15f;
  t3d_mat4fp_from_srt_euler(modelMatFP,
    (float[3]){modelScale, modelScale, modelScale},
    (float[3]){0,0,0},
    (float[3]){0,0,0}
  );

  T3DModelState modelState = t3d_model_state_create();
  auto it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);

  while(t3d_model_iter_next(&it)) {
    rspq_block_begin();
    auto mat = it.object->material;

    bool hasFresnel = strcmp(mat->name, "fres") == 0;
    // store the light-setup ID in one of the user values
    it.object->userValue0 = hasFresnel ? 1 : 0;

    t3d_model_draw_material(it.object->material, &modelState);
    t3d_model_draw_object(it.object, NULL);
    it.object->userBlock = rspq_block_end();
  }

  float camRotX = 2.5f;
  float camRotY = 0.24f;

  uint8_t colorAmbient[4] = {0x19, 0x19, 0x19, 0x00};
  uint8_t colorDir[4] = {0x1F, 0x1F, 0x1F, 0};
  T3DVec3 lightDirVec{0.0f, 1.0f, 0.0f};

  uint8_t lightPointColor[4] = {0xFF, 0x77, 0xFF, 0};
  T3DVec3 lightPointPos = {{4.0f, 10.0f, 0.0f}};

  uint8_t lightPointColor2[4] = {0x77, 0x77, 0xFF, 0};
  T3DVec3 lightPointPos2 = {{4.0f, 9.0f, 0.0f}};

  uint32_t frameIdx = 0;
  bool showMenu = true;
  float lightAngle = 0.0f;

  for(uint64_t frame = 0;; ++frame)
  {
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    auto pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;
    if(pressed.start)showMenu = !showMenu;

    float deltaTime = display_get_delta_time();

    lightAngle += deltaTime * 1.5f;
    lightPointPos.x = fm_cosf(lightAngle) * 40.0f;
    lightPointPos.z = fm_sinf(lightAngle) * 40.0f;

    lightPointPos2.x = fm_cosf(lightAngle * -1.2f) * 35.0f;
    lightPointPos2.z = fm_sinf(lightAngle * -1.2f) * 35.0f;

    {
      float camSpeed = deltaTime * 0.5f;
      float camRotSpeed = deltaTime * 0.015f;

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

      camTarget.v[0] = camPos.v[0] + camDir.v[0];
      camTarget.v[1] = camPos.v[1] + camDir.v[1];
      camTarget.v[2] = camPos.v[2] + camDir.v[2];
    }

    uint32_t currIdx = frame % 3;
    uint32_t nextIdx = (frame + 1) % 3;
    t3d_viewport_set_projection(&viewport[nextIdx], T3D_DEG_TO_RAD(85.0f), 2.5f, 100.0f);
    t3d_viewport_look_at(viewport[nextIdx], camPos, camTarget, {0,1,0});

    t3d_segment_address(1, (void*)(sizeof(T3DMat4FP) * currIdx));

    // ----------- DRAW ------------ //

    //uint64_t ticks = get_ticks();
    //exposure_set(fb);
    //ticks = get_ticks() - ticks;

    fb = display_get();
    rdpq_attach(fb, display_get_zbuf());
    postProc[(frameIdx+2) % BUFF_COUNT].attachHDR();
    //postProc[frameIdx].attachHDR();

    t3d_frame_start();
    rdpq_mode_antialias(AA_NONE);
    rdpq_mode_dithering(DITHER_NONE_NONE);
    rdpq_set_prim_color({0xFF, 0xFF, 0xFF, 0xFF});

    //float envPower = fm_sinf(lightAngle * 2.2f) * 0.5f + 0.5f;
    //envPower = (envPower * 0.5f) + 0.5f;
    float envPower = 1.0f;

    rdpq_set_env_color({
      (uint8_t)(0xFF*envPower),
      (uint8_t)(0xAA*envPower),
      (uint8_t)(0xEE*envPower),
      0xFF
    });
    rdpq_mode_fog(0);

    t3d_viewport_attach(&viewport[currIdx]);
    t3d_screen_clear_depth();
    t3d_screen_clear_color({0x0F, 0x0F, 0x0F, 0xFF});
    t3d_fog_set_enabled(false);

    t3d_light_set_ambient(colorAmbient);
    //t3d_light_set_directional(0, colorDir, lightDirVec); // optional directional light, can be disabled

    t3d_matrix_push(modelMatFP);

    uint8_t lastLightID = 0xFF;
    it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);
    while(t3d_model_iter_next(&it))
    {
      if(it.object->userValue0 != lastLightID) {
        lastLightID = it.object->userValue0;
        if(lastLightID == 0) {
          t3d_light_set_point(0, lightPointColor, lightPointPos, 0.09f, false);
          t3d_light_set_point(1, lightPointColor2, lightPointPos2, 0.07f, false);
          t3d_light_set_count(2);
        } else {
          uint8_t col0[4]{0x00, 0x00, 0x00, 0xF0};
          uint8_t col1[4]{0x00, 0x00, 0x00, 0x90};
          t3d_light_set_point(0, col0, {{camPos.x, camPos.y+0.1f, camPos.z+0.1f}}, 1.0f, false);
          t3d_light_set_point(1, col1, {{camPos.x, camPos.y+0.1f, camPos.z+0.1f}}, 1.0f, false);
          t3d_light_set_count(2);
        }
      }
      rspq_block_run(it.object->userBlock);
    }
    t3d_matrix_pop(1);

    auto surfBlur = postProc[frameIdx].hdrBloom(*fb, state.ppConf);

    rdpq_sync_pipe();
    rdpq_set_color_image(fb);

    // Debug: show last offscreen buffer (downscaled and/or blur)
    if(state.showOffscreen)
    {
      rdpq_sync_tile();
      rdpq_sync_load();

      rdpq_set_mode_standard();
      rdpq_mode_combiner(RDPQ_COMBINER_TEX);
      rdpq_mode_blender(0);
      rdpq_mode_antialias(AA_NONE);
      rdpq_mode_filter(FILTER_POINT);

      rdpq_blitparms_t param{};
      param.scale_x = 4.0f;
      param.scale_y = 4.0f;
      rdpq_tex_blit(&surfBlur, 0, 0, &param);
    }

    Debug::printStart();
    if(showMenu) {
      DebugMenu::draw(state);
    } else {
      Debug::printf(20, 20, "%.2f", display_get_fps());
    }

    /*Debug::printf(260, 220, "%.2f", display_get_fps());
    Debug::printf(16, 220, "F: %.2f", state.ppConf.hdrFactor);
    Debug::printf(16, 220-16, "Blurs: %d", state.ppConf.blurSteps);
    Debug::printf(16, 220-32, "BlurB: %.2f", state.ppConf.blurBrightness);
*/
    rdpq_detach_show();

    frameIdx = (frameIdx+1) % BUFF_COUNT;
  }
}
