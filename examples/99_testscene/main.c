#include <libdragon.h>
#include <rspq_profile.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

#include <stdarg.h>

#define RCP_TICKS_TO_USECS(ticks) (((ticks) * 1000000ULL) / RCP_FREQUENCY)
#define PERCENT(fraction, total) ((total) > 0 ? (float)(fraction) * 100.0f / (float)(total) : 0.0f)

static rspq_profile_data_t profile_data;

static void debug_printf_screen(float x, float y, const char* str, ...)
{
    char buf[512];
    size_t n = sizeof(buf);

    va_list va;
    va_start(va, str);
    char *buf2 = vasnprintf(buf, &n, str, va);
    va_end(va);

    t3d_debug_print(x, y, buf2);
}

static void rspq_profile_dump_overlay_screen(size_t index, uint64_t frame_avg, const char *name, float *posY)
{
    uint64_t mean = profile_data.slots[index].total_ticks / profile_data.frame_count;
    uint64_t mean_us = RCP_TICKS_TO_USECS(mean);
    float relative = PERCENT(mean, frame_avg);

    char buf[64];
    sprintf(buf, "%3.2f%%", relative);

    debug_printf_screen(24, *posY, "%-10s %6llu %7llu# %8s",
        name,
        profile_data.slots[index].sample_count / profile_data.frame_count,
        mean_us,
        buf);

    *posY += 10;
}

void rspq_profile_dump_screen()
{
    if (profile_data.frame_count == 0)
        return;

    uint64_t frame_avg = profile_data.total_ticks / profile_data.frame_count;
    uint64_t frame_avg_us = RCP_TICKS_TO_USECS(frame_avg);

    uint64_t counted_time = 0;
    for (size_t i = 0; i < RSPQ_PROFILE_SLOT_COUNT; i++) counted_time += profile_data.slots[i].total_ticks;

    // The counted time could be slightly larger than the total time due to various measurement errors
    uint64_t overhead_time = profile_data.total_ticks > counted_time ? profile_data.total_ticks - counted_time : 0;
    uint64_t overhead_avg = overhead_time / profile_data.frame_count;
    uint64_t overhead_us = RCP_TICKS_TO_USECS(overhead_avg);

    float overhead_relative = PERCENT(overhead_avg, frame_avg);

    uint64_t rdp_busy_avg = profile_data.rdp_busy_ticks / profile_data.frame_count;
    uint64_t rdp_busy_us = RCP_TICKS_TO_USECS(rdp_busy_avg);
    float rdp_utilisation = PERCENT(rdp_busy_avg, frame_avg);

    float posY = 20;
    for (size_t i = 0; i < RSPQ_PROFILE_SLOT_COUNT; i++)
    {
        if (profile_data.slots[i].name == NULL)
            continue;

        rspq_profile_dump_overlay_screen(i, frame_avg, profile_data.slots[i].name, &posY);
    }
    float posYInc = 10;
    posY += posYInc;
    debug_printf_screen(24, posY, "Frames   : %7lld", profile_data.frame_count); posY += posYInc;
    debug_printf_screen(24, posY, "FPS      : %7.1f", (float)RCP_FREQUENCY/(float)frame_avg); posY += posYInc;
    debug_printf_screen(24, posY, "Avg frame: %7lld#", frame_avg_us); posY += posYInc;
    debug_printf_screen(24, posY, "RDP busy : %7lld# (%2.2f%%)", rdp_busy_us, rdp_utilisation); posY += posYInc;
    debug_printf_screen(24, posY, "Unrec.   : %7lld# (%2.2f%%)", overhead_us, overhead_relative);
}

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

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  surface_t depthBuffer = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

  rdpq_init();
  rspq_profile_start();
  //rdpq_debug_start();
  //rdpq_debug_log(true);

  joypad_init();
  t3d_init();

  t3d_debug_print_init();
  sprite_t *spriteLogo = sprite_load("rom:/logo.ia8.sprite");
  T3DModel *model = t3d_model_load("rom://scene.t3dm");

  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  t3d_mat4_identity(&modelMat);
  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  float camScale = 0.5f;
  T3DVec3 camPos = {{-2.9232f, 67.6248f, 61.1093f}};

  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};

  float camRotX = 1.5968f;
  float camRotY = 4.0500f;

  uint8_t colorAmbient[4] = {190, 190, 150, 0xFF};
  //uint8_t colorAmbient[4] = {40, 40, 40, 0xFF};
  uint8_t colorDir[4]     = {0xFF, 0xFF, 0xFF, 0xFF};

  T3DVec3 lightDirVec = {{0.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  float rotAngle = 0.0f;
  T3DVec3 rotAxis = {{1.0f, 2.5f, 0.25f}};
  t3d_vec3_norm(&rotAxis);

  rspq_block_t *dplDraw = NULL;

  double lastTimeMs = 0;
  float time = 0.0f;

  bool requestDisplayMetrics = false;
  bool displayMetrics = false;

  for(uint64_t frame = 0;; ++frame)
  {
    uint64_t timeDraw = get_ticks_us();

    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    double nowMs = (double)get_ticks_us() / 1000.0;
    float deltaTime = (float)(nowMs - lastTimeMs);
    lastTimeMs = nowMs;
    time += deltaTime;

    {
      float camSpeed = deltaTime * 0.001f;
      float camRotSpeed = deltaTime * 0.00001f;

      camDir.v[0] = fm_cosf(camRotX) * fm_cosf(camRotY);
      camDir.v[1] = fm_sinf(camRotY);
      camDir.v[2] = fm_sinf(camRotX) * fm_cosf(camRotY);
      t3d_vec3_norm(&camDir);

      if(joypad.btn.z) {
        camRotX -= (float)joypad.stick_x * camRotSpeed;
        camRotY += (float)joypad.stick_y * camRotSpeed;
      } else {
        camPos.v[0] += camDir.v[0] * (float)joypad.stick_y * camSpeed;
        camPos.v[1] += camDir.v[1] * (float)joypad.stick_y * camSpeed;
        camPos.v[2] += camDir.v[2] * (float)joypad.stick_y * camSpeed;

        camPos.v[0] += camDir.v[2] * (float)joypad.stick_x * camSpeed;
        camPos.v[2] -= camDir.v[0] * (float)joypad.stick_x * camSpeed;
      }

      if(joypad.btn.c_up)camPos.v[1] += camSpeed * 15.0f;
      if(joypad.btn.c_down)camPos.v[1] -= camSpeed * 15.0f;

      camTarget.v[0] = camPos.v[0] + camDir.v[0];
      camTarget.v[1] = camPos.v[1] + camDir.v[1];
      camTarget.v[2] = camPos.v[2] + camDir.v[2];
    }

    if(joypad.btn.b) {
      requestDisplayMetrics = true;
      //colorAmbient[2] = colorAmbient[1] = colorAmbient[0] = 60;
      //colorDir[2] = colorDir[1] = colorDir[0] = 40;
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

    bool useReject = !joypad.btn.z;

    rotAngle += 0.03f;

    // Model-Matrix, t3d offers some basic matrix functions
    float modelScale = 0.15f;
    t3d_mat4_identity(&modelMat);
    //t3d_mat4_rotate(&modelMat, &rotAxis, rotAngle);
    t3d_mat4_scale(&modelMat, modelScale, modelScale, modelScale);
    t3d_mat4_to_fixed(modelMatFP, &modelMat);

    // ----------- DRAW ------------ //
    rdpq_attach(display_get(), &depthBuffer);

    t3d_frame_start(); // call this once per frame at the beginning of your draw function
    rdpq_set_prim_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});

    t3d_screen_set_size(display_get_width(), display_get_height(), 2, false);
    t3d_screen_clear_color(RGBA32(0, 0, 0, 0xFF));
    t3d_screen_clear_depth();

    t3d_projection_perspective(T3D_DEG_TO_RAD(85.0f), 2.0f, 150.0f);
    t3d_camera_look_at(&camPos, &camTarget); // convenience function to set camera matrix and related settings

    t3d_fog_set_range(17.0f, 100.0f);

    t3d_light_set_ambient(colorAmbient); // one global ambient light, always active
    t3d_light_set_directional(0, colorDir, &lightDirVec); // optional directional light, can be disabled
    t3d_light_set_count(1);

    // t3d functions can be recorded into a display list:
    if(!dplDraw) {
      rspq_block_begin();

      t3d_model_draw(model);

      dplDraw = rspq_block_end();
    }

    t3d_matrix_set_mul(modelMatFP, 1, 0);

    // ----------- TIME ------------ //

   /*if(joypad.btn.b) { // Wireframe
      rdpq_mode_antialias(AA_STANDARD);
      rdpq_mode_blender(RDPQ_BLENDER((MEMORY_RGB, FOG_ALPHA, IN_RGB, MEMORY_CVG)));
    } else {
      rdpq_mode_antialias(AA_STANDARD);
      rdpq_mode_blender(0);
    }*/

    //rspq_wait();
    //uint64_t timeDraw = get_ticks_us();

      rspq_block_run(dplDraw);
      //t3d_model_draw(model);

    //rspq_wait();
    //timeDraw = get_ticks_us() - timeDraw;

    // ---------------------------- //

    //t3d_mat_read(buffDebug);
    //rsp_wait();

    timeDraw = get_ticks_us() - timeDraw;

    if(displayMetrics)
    {
      //debugf("========= DRAW OVERLAY =========\n");
      //uint16_t *buffDebug = malloc_uncached(16);
      //rspq_wait();
      //t3d_mat_read(buffDebug);

      t3d_debug_print_start();

      // show pos / rot
      //debug_printf_screen(24, 190, "Pos: %.4f %.4f %.4f", camPos.v[0], camPos.v[1], camPos.v[2]);
      //debug_printf_screen(24, 200, "Rot: %.4f %.4f", camRotX, camRotY);

      debug_printf_screen(140, 218, "FPS: %.4f", display_get_fps());

      rspq_profile_dump_screen();
      //rspq_profile_reset();
      //debug_printf_screen(24, 180, "[Tris] screen: %d clip: %d", buffDebug[0], buffDebug[1]);
      //free_uncached(buffDebug);
      rdpq_mode_combiner(RDPQ_COMBINER_TEX);
      rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
      rdpq_sprite_blit(spriteLogo, 26.0f, 206.0f, NULL);
    }

    rdpq_detach_show();
    rspq_profile_next_frame();

    if(frame == 30)
    {
      if(!displayMetrics) {
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
