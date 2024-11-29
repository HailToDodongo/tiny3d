#include <libdragon.h>
#include <rspq_profile.h>

#include <t3d/t3d.h>
#include <t3d/t3ddebug.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/tpx.h>

#include "partSim.h"

/**
 * Demo showcasing textured particles.
 * NOTE: If this is the first demo you see of particles, please checkout '18_particles' first
 * as this will build upon that and doesn't go into any detail about the basics.
 *
 * Texture particles extend colored particles by letting you use a full-size or tiled texture.
 * No UV's or any texture rotations are supported, only tile based animations.
 */

#define RCP_TICKS_TO_USECS(ticks) (((ticks) * 1000000ULL) / RCP_FREQUENCY)
#define MIN(a,b) ((a) < (b) ? (a) : (b))
static const char* EXAMPLE_NAMES[] = {"Random"};

[[noreturn]]
int main()
{
	debug_init_isviewer();
	debug_init_usblog();

  asset_init_compression(2);
  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

  rdpq_init();
  //rdpq_debug_start();

  #if RSPQ_PROFILE
    rspq_profile_data_t profile_data = (rspq_profile_data_t){};
    uint64_t rdpTimeBusy = 0;
    uint64_t rspTimeTPX = 0;
    rspq_profile_start();
  #endif

  joypad_init();
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  t3d_init((T3DInitParams){});
  // Init tpx here, same as t3d, you can pass in a custom matrix-stack size if needed.
  tpx_init((TPXInitParams){});

  t3d_debug_print_init();


  uint32_t particleCountMax = 100'000;
  uint32_t particleCount = 2;
  uint32_t allocSize = sizeof(TPXParticle) * particleCountMax / 2;
  TPXParticle *particles = malloc_uncached(allocSize);
  debugf("Particle-Buffer %ldkb\n", allocSize / 1024);
  generate_particles_random(particles, particleCount);

  //sprite_t *texTest = sprite_load("rom://coin.i4.sprite");
  sprite_t *texTest = sprite_load("rom://swirl.i4.sprite");

  T3DModel *model = t3d_model_load("rom://scene.t3dm");
  rspq_block_begin();
    t3d_model_draw(model);
  model->userBlock = rspq_block_end();

  T3DMat4FP *matFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP *matPartFP = malloc_uncached(sizeof(T3DMat4FP));

  t3d_mat4fp_from_srt_euler(matFP,
    (float[]){0.25f, 0.25f, 0.25f},
    (float[]){0,0,0},
    (float[]){0,-1,0}
  );

  T3DVec3 camPos = {{-50.0f, 20.0f, 0.0f}};
  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};
  float camRotX = 0.0f;
  float camRotY = -0.2f;
  bool showModel = false;
  uint32_t example = 0;

  uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0xFF};
  T3DVec3 lightDirVec = {{0.0f, 0.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  T3DViewport viewport = t3d_viewport_create();
  float partSizeX = 0.9f;
  float partSizeY = 0.9f;

  float partMatScaleVal = 0.3f;
  T3DVec3 particleMatScale = {{1, 1, 1}};
  T3DVec3 particlePos = {{0, 0, 0}};
  T3DVec3 particleRot = {{0, 0, 0}};
  float time = 0;
  bool needRebuild = true;
  int spriteIdx = 0;

  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();
    float deltaTime = display_get_delta_time();

    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    if(joypad.btn.a)partMatScaleVal += deltaTime * 0.6f;
    if(joypad.btn.b)partMatScaleVal -= deltaTime * 0.6f;

    if(joypad.btn.c_right)partSizeX += deltaTime * 0.6f;
    if(joypad.btn.c_left)partSizeX -= deltaTime * 0.6f;
    if(joypad.btn.c_up)partSizeY += deltaTime * 0.6f;
    if(joypad.btn.c_down)partSizeY -= deltaTime * 0.6f;

    partSizeX = fmaxf(0.01f, fminf(1.0f, partSizeX));
    partSizeY = fmaxf(0.01f, fminf(1.0f, partSizeY));

    if(joypad.btn.d_left) { particleCount -= joypad.btn.z ? 200 : 20; needRebuild = true; }
    if(joypad.btn.d_right) { particleCount += joypad.btn.z ? 200 : 20; needRebuild = true; }
    if(particleCount < 2)particleCount = 2;
    if(particleCount > 0xFFFFFF)particleCount = 0;
    if(particleCount > particleCountMax)particleCount = particleCountMax;

    if(btn.l) { example -= 1; needRebuild = true; }
    if(btn.r) { example += 1; needRebuild = true; }
    if(example > 2)example = 0;

    if(btn.start)showModel = !showModel;
    {
      float camSpeed = deltaTime * 0.2f;
      float camRotSpeed = deltaTime * 0.02f;

      camDir.v[0] = fm_cosf(camRotX) * fm_cosf(camRotY);
      camDir.v[1] = fm_sinf(camRotY);
      camDir.v[2] = fm_sinf(camRotX) * fm_cosf(camRotY);
      t3d_vec3_norm(&camDir);

      if(joypad.btn.z) {
        camRotX += (float)joypad.stick_x * camRotSpeed;
        camRotY -= (float)joypad.stick_y * camRotSpeed;
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

    switch(example)
    {
      case 0: // Random
        time += deltaTime * 1.0f;
        //particleRot = (T3DVec3){{time,time*0.77f,time*1.42f}};
        particleMatScale = (T3DVec3){{partMatScaleVal, partMatScaleVal, partMatScaleVal}};

        if(needRebuild)generate_particles_random(particles, particleCount);
        rdpq_set_env_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
      break;
      case 1: // Flame
        particleRot = (T3DVec3){{0,0,0}};
        if(!joypad.btn.z)time += deltaTime * 1.0f;
        particleCount = 128;
        float posX = fm_cosf(time) * 80.0f;
        float posZ = fm_sinf(2*time) * 40.0f;

        simulate_particles_fire(particles, particleCount, posX, posZ);
        particleMatScale = (T3DVec3){{0.9f, partMatScaleVal, 0.9f}};
        particlePos.y = partMatScaleVal * 130.0f;
        rdpq_set_env_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
      break;
      case 2: // Grass
        time += deltaTime * 1.0f;
        particleRot = (T3DVec3){{0,0,0}};
        particlePos.y = 0;
        if(needRebuild) {
          particleCount = simulate_particles_grass(particles, particleCount);
        }
        particleMatScale = (T3DVec3){{partMatScaleVal, partSizeY * 2.9f, partMatScaleVal}};
        rdpq_set_env_color(blend_colors(
          (color_t){0xAA, 0xFF, 0x55, 0xFF},
          (color_t){0xFF, 0xAA, 0x55, 0xFF},
          fm_sinf(time)*0.5f+0.5f
        ));
      break;
    }
    needRebuild = false;

    t3d_mat4fp_from_srt_euler(matPartFP, particleMatScale.v, particleRot.v, particlePos.v);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(80.0f), 5.0f, 250.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // ======== Draw (3D) ======== //

    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    rdpq_mode_antialias(AA_NONE);
    rdpq_mode_dithering(DITHER_NONE_NONE);

    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(30, 30, 30, 0));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(0);

    if(showModel) {
      t3d_matrix_push(matFP);
      rspq_block_run(model->userBlock);
      t3d_matrix_pop(1);
    }

    // ======== Draw (Particles) ======== //
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_set_mode_standard();
    rdpq_mode_zbuf(true, true);
    rdpq_mode_zoverride(true, 0, 0);
    rdpq_mode_filter(FILTER_POINT);
    rdpq_mode_alphacompare(10);

    rdpq_mode_combiner(RDPQ_COMBINER1((PRIM,0,TEX0,0), (0,0,0,TEX0)));

    // upload texture for the following particles
    rdpq_texparms_t p = {};
    p.s.repeats = REPEAT_INFINITE;
    p.t.repeats = REPEAT_INFINITE;
    p.s.mirror = true;
    p.t.mirror = true;
    p.s.scale_log = -2;
    p.t.scale_log = -2;
    rdpq_sprite_upload(TILE0, texTest, &p);

    tpx_state_from_t3d();
    tpx_matrix_push(matPartFP);
    tpx_state_set_scale(partSizeX, partSizeY);
    int idx = (int16_t)(fm_floorf(time*22.0f) * 32) % 512;
    tpx_state_set_tex_offset(idx);

    tpx_particle_draw_tex(particles, particleCount);

    tpx_matrix_pop(1);

    t3d_debug_print_start();
    t3d_debug_printf(20,  18, "[D] Particles: %ld", particleCount);
    t3d_debug_printf(20,  30, "[C] %.2f %.2f", partSizeX, partSizeY);
    t3d_debug_printf(220, 18, "FPS: %.2f", display_get_fps());

    #if RSPQ_PROFILE
      double timePerPart = 0;
      if(particleCount > 0) {
        timePerPart = (double)rspTimeTPX / (double)particleCount * 1000;
      }
      t3d_debug_printf(20, 240-34, "RSP/tpx: %6lldus %.1f", rspTimeTPX, timePerPart);
      //t3d_debug_printf(20, 240-34, "RSP/tpx: %6lldus", rspTimeTPX);
      t3d_debug_printf(20, 240-24, "RDP    : %6lldus", rdpTimeBusy);
    #else
      t3d_debug_printf(20, 240-24, "[L/R]: %s", EXAMPLE_NAMES[example]);
    #endif

    rdpq_detach_show();

    #if RSPQ_PROFILE
      rspq_profile_next_frame();
      if(++profile_data.frame_count == 30) {
        rspq_profile_get_data(&profile_data);
        rspq_profile_reset();

        rdpTimeBusy = RCP_TICKS_TO_USECS(profile_data.rdp_busy_ticks / profile_data.frame_count);
        rspTimeTPX = RCP_TICKS_TO_USECS(profile_data.slots[3].total_ticks / profile_data.frame_count);

        profile_data.frame_count = 0;
      }
    #endif
  }
}

