/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <rspq_constants.h>
#include <rspq_profile.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

#include "main.h"
#include "debugMenu.h"
#include "postProcess.h"
#include "render/debugDraw.h"
#include "rsp/rspFX.h"
#include "scenes/scene.h"
#include "scenes/sceneMain.h"

namespace {
  constexpr int BUFF_COUNT = 3;

  State state{
    .ppConf = {
      .blurSteps = 4,
      .blurBrightness = 1.0f,
      .hdrFactor = 1.5f,
      .bloomThreshold = 0.2f,
      .scalingUseRDP = true,
     },
    .showOffscreen = false,
  };

  T3DVec3 camPos = {{-35.0, 21.0, 40.0}};
  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};

  rspq_profile_data_t profileData{};
  uint64_t lastUcodeTime = 0;
}

surface_t* fb = NULL;

[[noreturn]]
int main()
{
  profileData.frame_count = 0;
	debug_init_isviewer();
	debug_init_usblog();
	//asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, BUFF_COUNT, GAMMA_NONE, FILTERS_RESAMPLE);

  rdpq_init();
  #if RSPQ_PROFILE
    rspq_profile_start();
  #endif

  RspFX::init();
  PostProcess postProc[BUFF_COUNT]{};
  Debug::init();

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport[BUFF_COUNT];
  for(int i = 0; i < BUFF_COUNT; ++i) {
    viewport[i] = t3d_viewport_create();
  }

  Scene *scene = new SceneMain();

  float camRotX = -1.9f;
  float camRotY = 0.24f;

  uint8_t colorAmbient[4] = {0x19, 0x19, 0x19, 0x00};

  uint32_t frameIdx = 0;
  bool showMenu = true;

  for(uint64_t frame = 0;; ++frame)
  {
    uint32_t frameIdxLast = (frameIdx+BUFF_COUNT-1) % BUFF_COUNT;
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    auto pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;
    if(pressed.start)showMenu = !showMenu;

    float deltaTime = display_get_delta_time();

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

    scene->update(deltaTime);
    t3d_segment_address(1, (void*)(sizeof(T3DMat4FP) * currIdx));

    // ----------- DRAW ------------ //

    //uint64_t ticks = get_ticks();
    //exposure_set(fb);
    //ticks = get_ticks() - ticks;

    fb = display_get();
    rdpq_attach(fb, display_get_zbuf());

    postProc[frameIdx].setConf(state.ppConf);
    postProc[frameIdx].beginFrame();

    t3d_frame_start();
    rdpq_mode_antialias(AA_NONE);
    rdpq_mode_dithering(DITHER_NONE_NONE);
    rdpq_set_prim_color({0xFF, 0xFF, 0xFF, 0xFF});

    rdpq_mode_fog(0);

    t3d_viewport_attach(&viewport[currIdx]);
    t3d_screen_clear_depth();
    t3d_screen_clear_color({0x0F, 0x0F, 0x0F, 0xFF});
    t3d_fog_set_enabled(false);

    t3d_light_set_ambient(colorAmbient);

    scene->draw(deltaTime);

    postProc[frameIdx].endFrame();
    auto surfBlur = postProc[frameIdxLast].applyEffects(*fb);

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

    #if RSPQ_PROFILE
      Debug::printf(20, 220, "%.2fms", lastUcodeTime / 1000.0f);
    #endif

    rdpq_detach_show();

    #if RSPQ_PROFILE
      rspq_profile_next_frame();
      if(++profileData.frame_count == 30) {
        rspq_profile_get_data(&profileData);
        //rspq_profile_dump();
        rspq_profile_reset();

        for(auto &p : profileData.slots) {
          if(p.name && strcmp(p.name, "rsp_fx") == 0) {
            lastUcodeTime = (((p.total_ticks) * 1000000ULL) / RCP_FREQUENCY) / profileData.frame_count;
            break;
          }
        }
        profileData.frame_count = 0;
      }
    #endif

    frameIdx = (frameIdx+1) % BUFF_COUNT;
  }
}
