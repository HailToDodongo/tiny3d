#include <libdragon.h>
#include <rspq_profile.h>

#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/tpx.h>

#define RCP_TICKS_TO_USECS(ticks) (((ticks) * 1000000ULL) / RCP_FREQUENCY)

static void generateParticles(TPXParticle *particles, int count) {
  for (int i = 0; i < count; i++) {
    int p = i / 2;
    int8_t *ptPos = i % 2 == 0 ? particles[p].posA : particles[p].posB;
    uint8_t *ptColor = i % 2 == 0 ? particles[p].colorA : particles[p].colorB;

    particles[p].sizeA = 20 + (rand() % 10);
    particles[p].sizeB = 20 + (rand() % 10);

    T3DVec3 pos = {{
       (i * 1 + rand()) % 64 - 32,
       (i * 3 + rand()) % 64 - 32,
       (i * 4 + rand()) % 64 - 32
     }};

    t3d_vec3_norm(&pos);
    float len = rand() % 40;
    pos.v[0] *= len;
    pos.v[1] *= len;
    pos.v[2] *= len;

    ptPos[0] = (rand() % 256) - 128;
    ptPos[1] = (rand() % 256) - 128;
    ptPos[2] = (rand() % 256) - 128;

    ptColor[3] = rand() % 8;
    ptColor[0] = 25 + (rand() % 230);
    ptColor[1] = 25 + (rand() % 230);
    ptColor[2] = 25 + (rand() % 230);
  }
}

static void gradient_fire(uint8_t *color, float t) {
    t = fminf(1.0f, fmaxf(0.0f, t));
    t = 0.8f - t;
    t *= t;

    if (t < 0.25f) {
        // Dark red to bright red
        color[0] = (uint8_t)(200 * (t / 0.25f)) + 55;
        color[1] = 0;
        color[2] = 0;
    } else if (t < 0.5f) {
        // Bright red to yellow
        color[0] = 255;
        color[1] = (uint8_t)(255 * ((t - 0.25f) / 0.25f));
        color[2] = 0;
    } else if (t < 0.75f) {
        // Yellow to white (optional, if you want a bright white center)
        color[0] = 255;
        color[1] = 255;
        color[2] = (uint8_t)(255 * ((t - 0.5f) / 0.25f));
    } else {
        // White to black
        color[0] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
        color[1] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
        color[2] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
    }
}

static int currentPart  = 0;

static void simulate_particles(TPXParticle *particles, int partCount, float posX, float posZ) {
  int p = currentPart / 2;
  if(currentPart % (1+(rand() % 3)) == 0) {
    int8_t *ptPos = currentPart % 2 == 0 ? particles[p].posA : particles[p].posB;
    int8_t *size = currentPart % 2 == 0 ? &particles[p].sizeA : &particles[p].sizeB;
    uint8_t *color = currentPart % 2 == 0 ? particles[p].colorA : particles[p].colorB;

    ptPos[0] = posX + (rand() % 16) - 8;
    ptPos[1] = -126;
    gradient_fire(color, 0);
    ptPos[2] = posZ + (rand() % 16) - 8;
    *size = 60 + (rand() % 10);
  }
  currentPart = (currentPart + 1) % partCount;

  // move all up by one unit
  for (int i = 0; i < partCount/2; i++) {
    gradient_fire(particles[i].colorA, (particles[i].posA[1] + 127) / 150.0f);
    gradient_fire(particles[i].colorB, (particles[i].posB[1] + 127) / 150.0f);

    particles[i].posA[1] += 1;
    particles[i].posB[1] += 1;
    if(currentPart % 4 == 0) {
      particles[i].sizeA -= 2;
      particles[i].sizeB -= 2;
      if(particles[i].sizeA < 0)particles[i].sizeA = 0;
      if(particles[i].sizeB < 0)particles[i].sizeB = 0;
    }
  }
}

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

  rspq_profile_data_t profile_data = (rspq_profile_data_t){};
  uint64_t rdpTimeBusy = 0;
  uint64_t rspTimeTPX = 0;
  #if RSPQ_PROFILE
    rspq_profile_start();
  #endif

  joypad_init();
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  t3d_init((T3DInitParams){});
  tpx_init((TPXInitParams){});

  T3DModel *model = t3d_model_load("rom://scene.t3dm");
  rspq_block_begin();
    t3d_model_draw(model);
  model->userBlock = rspq_block_end();

  T3DMat4FP *matFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP *matPartFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DVec3 camPos = {{3.0f, 65.0f, 65.0f}};
  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};
  float camRotX = 1.5;
  float camRotY = 4.0f;
  bool showModel = false;
  int example = 0;

  int batchSize = 344;
  int batchCountMax = 500;
  int batchCount = 2;
  int particleCount = batchSize * batchCountMax;
  TPXParticle *particles = malloc_uncached(sizeof(TPXParticle) * particleCount/2);
  generateParticles(particles, particleCount);

  uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0xFF};
  T3DVec3 lightDirVec = {{0.0f, 0.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  T3DViewport viewport = t3d_viewport_create();
  float rotAngle = 0.0f;
  float partScale = 0.25f;
  float partSize = 0.15f;

  T3DVec3 flameScale;
  T3DVec3 flamePos = {{0,0,0}};

  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();
    float deltaTime = display_get_delta_time();
    rotAngle += deltaTime * 0.1f;

    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    if(joypad.btn.a)partScale += deltaTime * 0.6f;
    if(joypad.btn.b)partScale -= deltaTime * 0.6f;
    if(joypad.btn.d_up)partSize += deltaTime * 0.6f;
    if(joypad.btn.d_down)partSize -= deltaTime * 0.6f;
    partSize = fmaxf(0.01f, fminf(1.0f, partSize));

    if(joypad.btn.d_left)batchCount -= joypad.btn.z ? 10 : 1;
    if(joypad.btn.d_right)batchCount += joypad.btn.z ? 10 : 1;
    if(batchCount < 1)batchCount = 1;
    if(batchCount > batchCountMax)batchCount = batchCountMax;

    if(btn.c_left || btn.c_right) {
      example = example == 1 ? 0 : 1;
      generateParticles(particles, particleCount);
    }

    if(btn.start)showModel = !showModel;
    {
      float flameSpeed = deltaTime * 2.1f;
      float camSpeed = deltaTime * 1.1f;
      float camRotSpeed = deltaTime * 0.02f;

      camDir.v[0] = fm_cosf(camRotX) * fm_cosf(camRotY);
      camDir.v[1] = fm_sinf(camRotY);
      camDir.v[2] = fm_sinf(camRotX) * fm_cosf(camRotY);
      t3d_vec3_norm(&camDir);

      if(joypad.btn.z) {
        camRotX += (float)joypad.stick_x * camRotSpeed;
        camRotY += (float)joypad.stick_y * camRotSpeed;
      } else {
        if(joypad.btn.l) {
          flamePos.v[0] += camDir.v[0] * (float)joypad.stick_y * flameSpeed;
          flamePos.v[2] += camDir.v[2] * (float)joypad.stick_y * flameSpeed;
          flamePos.v[0] += camDir.v[2] * (float)joypad.stick_x * -flameSpeed;
          flamePos.v[2] -= camDir.v[0] * (float)joypad.stick_x * -flameSpeed;

        } else {
          camPos.v[0] += camDir.v[0] * (float)joypad.stick_y * camSpeed;
          camPos.v[1] += camDir.v[1] * (float)joypad.stick_y * camSpeed;
          camPos.v[2] += camDir.v[2] * (float)joypad.stick_y * camSpeed;

          camPos.v[0] += camDir.v[2] * (float)joypad.stick_x * -camSpeed;
          camPos.v[2] -= camDir.v[0] * (float)joypad.stick_x * -camSpeed;
        }
      }

      if(joypad.btn.c_up)camPos.v[1] += camSpeed * 45.0f;
      if(joypad.btn.c_down)camPos.v[1] -= camSpeed * 45.0f;

      camTarget.v[0] = camPos.v[0] + camDir.v[0];
      camTarget.v[1] = camPos.v[1] + camDir.v[1];
      camTarget.v[2] = camPos.v[2] + camDir.v[2];
    }

    if(example == 1) {
      //batchCount = 2;
      rotAngle = 0;
      simulate_particles(particles, batchSize * batchCount, flamePos.v[0], flamePos.v[2]);
      flameScale = (T3DVec3){{0.7f, partScale, 0.7f}};
    } else {
      flameScale = (T3DVec3){{partScale, partScale, partScale}};
    }

    particleCount = batchSize * batchCount;

    // we can set up our viewport settings beforehand here
    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(80.0f), 5.0f, 250.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf()); // set the target to draw to
    t3d_frame_start(); // call this once per frame at the beginning of your draw function

    rdpq_mode_antialias(AA_NONE);
    rdpq_mode_dithering(DITHER_NONE_NONE);

    t3d_viewport_attach(&viewport); // now use the viewport, this applies proj/view matrices and sets scissoring

    t3d_screen_clear_color(RGBA32(30, 30, 30, 0));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient); // one global ambient light, always active
    t3d_light_set_count(0);

    t3d_mat4fp_from_srt_euler(matPartFP,
      flameScale.v,
      (float[]){rotAngle,rotAngle*2.4f,rotAngle*1.3f},
      (float[]){0, example == 1 ? (100*partScale) : 0, 0}
    );
    float modelScale = 0.25f;
    t3d_mat4fp_from_srt_euler(matFP,
      (float[]){modelScale, modelScale, modelScale},
      (float[]){0,0,0},
      (float[]){0,-10,0}
    );
    t3d_matrix_push(matFP);
    if(showModel)rspq_block_run(model->userBlock);
    t3d_matrix_set(matPartFP, true);

    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_zbuf(true, true);
    rdpq_mode_zoverride(true, 0, 0);
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);

    tpx_state_from_t3d(); // copy the current state from t3d to tpx
    tpx_state_set_scale(partSize);

    for(int i = 0; i < batchCount; i++) {
      tpx_particle_draw(particles + (i * batchSize/2), batchSize);
    }

    t3d_matrix_pop(1);

    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 24, 30, "Particles: %d (%dx%d)", particleCount, batchCount, batchSize);
    //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 24, 40, "%.2f", partSize);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 240, 30, "FPS: %.2f", display_get_fps());

    #if RSPQ_PROFILE
      double timePerPart = (double)rspTimeTPX / (double)particleCount;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 24, 240-34, "RSP/tpx: %lldus (%.4f)", rspTimeTPX, timePerPart);
      rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 24, 240-34, "RSP/tpx: %lldus", rspTimeTPX);
      rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 24, 240-24, "RDP    : %lldus", rdpTimeBusy);
    #endif
//
    rdpq_detach_show();

    #if RSPQ_PROFILE
      rspq_profile_next_frame();
      if(++profile_data.frame_count == 30) {
        rspq_profile_get_data(&profile_data);
        //rspq_profile_dump();
        rspq_profile_reset();

        rdpTimeBusy = RCP_TICKS_TO_USECS(profile_data.rdp_busy_ticks / profile_data.frame_count);
        rspTimeTPX = RCP_TICKS_TO_USECS(profile_data.slots[3].total_ticks / profile_data.frame_count);

        profile_data.frame_count = 0;
      }
    #endif
  }
}

