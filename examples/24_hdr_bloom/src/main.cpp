#include <libdragon.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

#include "fbBlur.h"
#include "rspFX.h"

/**
 *
 */

surface_t* fb = NULL;

[[noreturn]]
int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
  auto surfHDR = surface_alloc(FMT_RGBA32, 320, 240);

  rdpq_init();

  RspFX::init();
  FbBlur fbBlur{};

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport[3];
  viewport[0] = t3d_viewport_create();
  viewport[1] = t3d_viewport_create();
  viewport[2] = t3d_viewport_create();

  t3d_debug_print_init();

  T3DModel *model = t3d_model_load("rom://scene.t3dm");
  rdpq_font_t* font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
  rdpq_text_register_font(1, font);

  auto gradTest = sprite_load("rom:/gradTest.rgba16.sprite");

  T3DMat4FP* modelMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

  float modelScale = 0.15f;
  t3d_mat4fp_from_srt_euler(modelMatFP,
    (float[3]){modelScale, modelScale, modelScale},
    (float[3]){0,0,0},
    (float[3]){0,0,0}
  );

  T3DObject *objSky = t3d_model_get_object(model, "Scene");
  T3DModelState state = t3d_model_state_create();

  rspq_block_begin();
    t3d_matrix_push(modelMatFP);

    rdpq_mode_zbuf(true, true);
    rdpq_mode_antialias(AA_NONE);
    t3d_model_draw_material(objSky->material, &state);
    t3d_model_draw_object(objSky, NULL);

    t3d_matrix_pop(1);
  model->userBlock = rspq_block_end();

  T3DVec3 camPos = {{10.0, 21.0, 40.0}};
  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};

  float camRotX = -1.14f;
  float camRotY = 0.24f;

  uint8_t colorAmbient[4] = {0x1f, 0x1f, 0x1f, 0xFF};
  uint8_t colorDir[4] = {0xFF, 0xFF, 0xFF, 0xFF};
  T3DVec3 lightDirVec{0.0f, 1.0f, 0.0f};

  bool showGradient = true;
  float hdrFactor = 2;

  for(uint64_t frame = 0;; ++frame)
  {
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    auto pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    if(pressed.start)showGradient = !showGradient;
    if(held.c_left)hdrFactor -= 0.1f;
    if(held.c_right)hdrFactor += 0.1f;
    if(hdrFactor < 0)hdrFactor = 0;

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

    rdpq_set_color_image(&surfHDR);

    t3d_frame_start();
    rdpq_mode_antialias(AA_REDUCED);
    rdpq_mode_dithering(DITHER_NONE_NONE);


    t3d_viewport_attach(&viewport[currIdx]);
    t3d_screen_clear_depth();
    t3d_screen_clear_color({0x22, 0x22, 0x30, 0xFF});

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, lightDirVec); // optional directional light, can be disabled
    t3d_light_set_count(1);

    rspq_block_run(model->userBlock);
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_sync_load();

    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
    rdpq_blitparms_s p{};
    p.scale_x = 8.0;
    p.scale_y = 2.0;

    if(showGradient)
    {
      constexpr int slices = 12;
      for(int i=0; i<=slices; ++i) {
        uint8_t prim = 0xFF - (i * (0xFF / slices));
        rdpq_set_prim_color({prim, prim, prim, prim});
        rdpq_sprite_blit(gradTest, 32, 16 + i*16, &p);
      }
    }


        //rdpq_set_prim_color({0xFF, 0xFF, 0xFF, 0xFF});
    //rdpq_tex_blit(&surfHDR, 0, 0, nullptr);
    // @TODO: ucode here

    // blur
    auto surfBlur = fbBlur.blur(surfHDR);


    rspq_wait();
    auto t = get_ticks();

    rspq_highpri_begin();
    RspFX::hdrBlit(surfHDR.buffer, fb->buffer, surfBlur.buffer, hdrFactor);
    rspq_highpri_end();
    rspq_flush();
    rspq_highpri_sync();
    t = get_ticks() - t;
    debugf("Time: %lldus\n", TICKS_TO_US(t));

    rdpq_set_color_image(fb);
    // Debug: scale up blur again
    if(!held.z)
    {
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

    rdpq_text_printf(NULL, 1, 260, 220, "%.2f", display_get_fps());
    rdpq_text_printf(NULL, 1, 16, 220, "F: %.2f", hdrFactor);
    //rdpq_text_printf(NULL, 1, 16, 24, "Bias: %.2f", exposure_bias);
    //rdpq_text_printf(NULL, 1, 16, 32, "Avg. Bright.: %.2f", average_brightness);

    //rdpq_set_mode_fill({0xFF, 0xFF, 0xFF, 0xFF});
    //rdpq_fill_rectangle(8, 8, 16, 16);

    rdpq_detach_show();

  }
}
