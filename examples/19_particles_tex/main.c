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
 *
 * NOTE: If this is the first demo you see of particles, please checkout '18_particles' first
 * as this will build upon that and doesn't go into any detail about the basics.
 *
 * Texture particles extend colored particles by letting you use a full-size or tiled texture.
 * No UV's or any texture rotations are supported, only tile based animations.
 * Details are explained in the demo, but to give an overview of the process / settings:
 *
 * A texture must be uploaded via RDPQ, the ucode itself never does any texture loading.
 * Each particle gets textured by creating UVs that make the entire texture cover the area.
 * No matter the scale, and even if non-uniform, it will always cover it even if squishing is needed.
 *
 * However the ucode doesn't know the actual size, so it will always map it to 8x8px UVs.
 * In order to use non 8x8px textures, you can adjust the tile-settings to scale them up or down.
 *
 * Each particle can also individually define an UV offset on the U-axis in 1/4th pixels steps.
 * Those too refer to the 8x8px base size and are stored in the alpha channel of the color.
 * On top, a global offset in the same space can be set, which is useful for animations.
 *
 * In order to aid with sprite-sheets that animate rotation (to simulate 3D rotation along Z),
 * a mirroring feature is available. This will only require half the amount of frames.
 * By specifying how many sprites you have, it will mirror and invert the order for you.
 *
 * As an example with a texture that has 4 frames, here is the order it would go if we visualize the mirroring:
 *            ________________#________________
 *   (origin) | 0 | 1 | 2 | 3 # - | - | - | - | (mirror-U)
 *            #################################
 * (mirror-V) | - | - | - | - # 7 | 6 | 5 | 4 | (mirror-UV)
 *            ----------------#----------------
 *
 * This is made more clear by looking at the example texture 'swirl.i4.png',
 * and the code use here to draw it.
 */

#define RCP_TICKS_TO_USECS(ticks) (((ticks) * 1000000ULL) / RCP_FREQUENCY)
#define MIN(a,b) ((a) < (b) ? (a) : (b))
static const char* EXAMPLE_NAMES[] = {"Random 8px", "Random 16px", "Random 32px", "Random 64px", "Fire", "Coins"};

#define FB_COUNT 3

[[noreturn]]
int main()
{
	debug_init_isviewer();
	debug_init_usblog();

  asset_init_compression(2);
  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, FB_COUNT, GAMMA_NONE, FILTERS_RESAMPLE);

  rdpq_init();
  //rdpq_debug_start();

  uint64_t rdpTimeBusy = 0;
  uint64_t rspTimeTPX = 0;
  #if RSPQ_PROFILE
    rspq_profile_data_t profile_data = (rspq_profile_data_t){};
    rspq_profile_start();
  #endif

  joypad_init();
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  t3d_init((T3DInitParams){});
  // Init tpx here, same as t3d, you can pass in a custom matrix-stack size if needed.
  tpx_init((TPXInitParams){});

  t3d_debug_print_init();

  uint32_t particleCountMax = 100'000;
  uint32_t particleCount = 1000;

  // There is no special struct for textured particles compared to colored ones.
  // The only difference is that the alpha channel of the color is used for the texture offset.
  // You can still define a global alpha value via the CC ofc.
  uint32_t allocSize = sizeof(TPXParticleS8) * particleCountMax / 2;
  TPXParticleS8 *particlesS8 = malloc_uncached(allocSize);
  debugf("Particle-Buffer %ldkb\n", allocSize / 1024);

  // Additionally, a 16bit version of particles is available.
  // This one takes up more space (24 bytes vs 16 bytes per pair) and is slightly slower.
  // In return, it can cover a larger range which can be useful for 3D sprites placed in a scene.
  // The 8bit variant should be preferred when possible (e.g. in local particle effects)
  allocSize = sizeof(TPXParticleS16) * particleCountMax / 2;
  TPXParticleS16 *particlesS16 = malloc_uncached(allocSize);

  sprite_t *texTest[] = {
      sprite_load("rom://tex8.i8.sprite"),
      sprite_load("rom://tex16.i8.sprite"),
      sprite_load("rom://tex32.i8.sprite"),
      sprite_load("rom://tex64.i8.sprite"),
      sprite_load("rom://swirl.i4.sprite"),
      sprite_load("rom://coin.i4.sprite"),
  };

  T3DModel *model = t3d_model_load("rom://scene.t3dm");
  rspq_block_begin();
    t3d_model_draw(model);
  model->userBlock = rspq_block_end();

  T3DMat4FP *matFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP *matPartFP = malloc_uncached(sizeof(T3DMat4FP) * FB_COUNT);

  t3d_mat4fp_from_srt_euler(matFP,
    (float[]){0.25f, 0.25f, 0.25f},
    (float[]){0,0,0},
    (float[]){0,-1,0}
  );

  T3DVec3 camPos = {{-80.0f, 40.0f, 0.0f}};
  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};
  float camRotX = 0.0f;
  float camRotY = -0.2f;
  bool showModel = false;
  uint32_t example = 0;

  uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0xFF};
  T3DVec3 lightDirVec = {{0.0f, 0.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  T3DViewport viewport = t3d_viewport_create_buffered(FB_COUNT);
  float partSizeX = 1.0f;
  float partSizeY = 1.0f;

  float partMatScaleVal = 1.3f;
  T3DVec3 particleMatScale = {{1, 1, 1}};
  T3DVec3 particlePos = {{0, 0, 0}};
  T3DVec3 particleRot = {{0, 0, 0}};
  float time = 0;
  float timeTile = 0;
  bool needRebuild = true;
  bool measureTime = false;
  int frameIdx = 0;

  for(;;)
  {
    // ======== Update ======== //
    frameIdx = (frameIdx + 1) % FB_COUNT;
    joypad_poll();
    float deltaTime = display_get_delta_time();

    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t released = joypad_get_buttons_released(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    if(joypad.btn.a)partMatScaleVal += deltaTime * 0.6f;
    if(joypad.btn.b)partMatScaleVal -= deltaTime * 0.6f;

    if(joypad.btn.c_right)partSizeX += deltaTime * 0.6f;
    if(joypad.btn.c_left)partSizeX -= deltaTime * 0.6f;
    if(joypad.btn.c_up)partSizeY += deltaTime * 0.6f;
    if(joypad.btn.c_down)partSizeY -= deltaTime * 0.6f;

#if RSPQ_PROFILE
    measureTime = joypad.btn.z;
#endif

    partSizeX = fmaxf(0.01f, fminf(1.0f, partSizeX));
    partSizeY = fmaxf(0.01f, fminf(1.0f, partSizeY));

    if(joypad.btn.d_left)particleCount -= joypad.btn.z ? 200 : 20;
    if(joypad.btn.d_right)particleCount += joypad.btn.z ? 200 : 20;
    if(released.d_left || released.d_right)needRebuild = true;
    if(particleCount < 2)particleCount = 2;
    if(particleCount > 0xFFFFFF)particleCount = 0;
    if(particleCount > particleCountMax)particleCount = particleCountMax;

    if(btn.l) { example -= 1; needRebuild = true; }
    if(btn.r) { example += 1; needRebuild = true; }
    if(example > 5)example = 0;

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

    bool is16Bit = false;
    bool isSpriteRot = false;
    switch(example)
    {
      case 4: // Flame
        particleRot = (T3DVec3){{0,0,0}};
        if(!joypad.btn.z)time += deltaTime * 1.0f;
        timeTile += deltaTime * 25.1f;
        particleCount = 128;
        float posX = fm_cosf(time) * 80.0f;
        float posZ = fm_sinf(2*time) * 40.0f;

        simulate_particles_fire(particlesS8, particleCount, posX, posZ);
        particleMatScale = (T3DVec3){{0.9f, partMatScaleVal, 0.9f}};
        particlePos.y = partMatScaleVal * 130.0f;
        rdpq_set_env_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
        isSpriteRot = true;
      break;
      case 5: // Coins
        time += deltaTime * 1.2f;
        timeTile += deltaTime * 20.0f;
        particleRot = (T3DVec3){{0,0,0}};
        particlePos.y = 0;
        if(needRebuild) {
          particleCount = simulate_particles_coins(particlesS16, particleCount);
        }
        particleMatScale = (T3DVec3){{partMatScaleVal, partSizeY * 2.9f, partMatScaleVal}};
        rdpq_set_env_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
        is16Bit = true;
      break;
      default: // Random
        time += deltaTime * 0.2f;
        timeTile += deltaTime * 0.1f;
        particleRot = (T3DVec3){{time,time*0.77f,time*1.42f}};
        particleMatScale = (T3DVec3){{partMatScaleVal, partMatScaleVal, partMatScaleVal}};

        if(needRebuild)generate_particles_random(particlesS8, particleCount);
        rdpq_set_env_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
        isSpriteRot = true;
      break;
    }
    needRebuild = false;

    t3d_mat4fp_from_srt_euler(&matPartFP[frameIdx], particleMatScale.v, particleRot.v, particlePos.v);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(80.0f), 5.0f, 250.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // ======== Draw (3D) ======== //

    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    rdpq_mode_antialias(AA_NONE);
    rdpq_mode_dithering(DITHER_NONE_NONE);

    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(60, 36, 71, 255));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(0);

    if(showModel) {
      rdpq_mode_zbuf(false, true);
      t3d_matrix_push(matFP);
      rspq_block_run(model->userBlock);
      t3d_matrix_pop(1);
    }

    // ======== Draw (Particles) ======== //
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_sync_load();
    rdpq_set_mode_standard();
    rdpq_mode_antialias(AA_NONE);
    rdpq_mode_zbuf(true, true);
    rdpq_mode_zoverride(true, 0, 0);
    rdpq_mode_filter(FILTER_POINT);
    rdpq_mode_alphacompare(10);

    rdpq_mode_combiner(RDPQ_COMBINER1((PRIM,0,TEX0,0), (0,0,0,TEX0)));

    // Upload texture for the following particles.
    // The ucode itself never loads or switches any textures,
    // so you can only use what you have uploaded before in a single draw call.
    rdpq_texparms_t p = {};
    p.s.repeats = REPEAT_INFINITE;
    p.t.repeats = REPEAT_INFINITE;
    // Texture UVs are internally always mapped to 8x8px tiles across the entire particle.
    // Even with non-uniform scaling, it is squished into that space.
    // In order to use differently sized textures (or sections thereof) adjust the scale here.
    // E.g.: scale_log = 4px=1, 8px=0, 16px=-1, 32px=-2, 64px=-3
    // This also means that you are limited to power-of-two sizes for a section of a texture.
    // You can however still have textures with a multiple of a power-of-two size on one axis.
    int logScale =  - __builtin_ctz(texTest[example]->height / 8);
    p.s.scale_log = logScale;
    p.t.scale_log = logScale;

    // For sprite sheets that animate a rotation, we can activate mirroring.
    // to only require half rotation to be animated.
    // This works by switching over to the double-mirrored section and repeating the animation,
    // which is handled internally in the ucode for you if enabled.
    p.s.mirror = isSpriteRot;
    p.t.mirror = isSpriteRot;
    rdpq_sprite_upload(TILE0, texTest[example], &p);

    tpx_state_from_t3d();
    tpx_matrix_push(&matPartFP[frameIdx]);
    tpx_state_set_scale(partSizeX, partSizeY);

    float tileIdx = fm_floorf(timeTile) * 32;
    if(tileIdx >= 512)timeTile = 0;

    switch(example)
    {
      // here we set the UV offset for a texture, and the point at which the double-mirror should happen
      // if we only want a static image, set the offset and mirror-point to 0.
      // Note that offset is in the 8x8px space.
      default: tpx_state_set_tex_params(0, 0); break;
      // the flame example uses a sprite-sheet with the mirror trick.
      // adjust the UV offset over time, and defines the mirror point.
      // The mirror point is defined as the amount of sections in the sprite-sheet.
      case 4: tpx_state_set_tex_params((int16_t)tileIdx, 8); break;
      // Coins, this also plays an animation, but without any mirroring.
      case 5: tpx_state_set_tex_params((int16_t)tileIdx, 0); break;
    }

    if(measureTime) {
      rspq_wait();
      rspq_highpri_begin();
      wait_ms(2);
      rspTimeTPX = get_ticks();
    }

    if(is16Bit) {
      tpx_particle_draw_tex_s16(particlesS16, particleCount);
    } else {
      tpx_particle_draw_tex_s8(particlesS8, particleCount);
    }

    if(measureTime)
    {
      rspq_highpri_end();
      rspq_highpri_sync();
      rspTimeTPX = get_ticks() - rspTimeTPX;
      rspTimeTPX = TICKS_TO_US(rspTimeTPX);
    }

    tpx_matrix_pop(1);

    t3d_debug_print_start();
    t3d_debug_printf(20,  18, "[D] Particles: %ld", particleCount);
    t3d_debug_printf(20,  30, "[C] %.2f %.2f", partSizeX, partSizeY);
    t3d_debug_printf(220, 18, "FPS: %.2f", display_get_fps());

    if(measureTime)
    {
      double timePerPart = 0;
      if(particleCount > 0) {
        timePerPart = (double)rspTimeTPX / (double)particleCount * 1000;
      }
      t3d_debug_printf(20, 240-34, "RSP/tpx: %6lldus %.1f", rspTimeTPX, timePerPart);
      //t3d_debug_printf(20, 240-34, "RSP/tpx: %6lldus", rspTimeTPX);
      t3d_debug_printf(20, 240-24, "RDP    : %6lldus", rdpTimeBusy);
    } else {
      t3d_debug_printf(20, 240-24, "[L/R]: %s", EXAMPLE_NAMES[example]);
    }

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

