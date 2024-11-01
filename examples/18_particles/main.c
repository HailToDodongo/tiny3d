#include <libdragon.h>

#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/tpx.h>

void generateParticles(TPXParticle *particles, int count, bool dynScale) {
  for (int i = 0; i < count; i++) {
    int p = i / 2;
    int8_t *ptPos = i % 2 == 0 ? particles[p].posA : particles[p].posB;
    uint8_t *ptColor = i % 2 == 0 ? particles[p].colorA : particles[p].colorB;

    if(dynScale) {
      particles[p].sizeA = 4 + (rand() % 60);
      particles[p].sizeB = 4 + (rand() % 60);
    } else {
      particles[p].sizeA = 4;
      particles[p].sizeB = 4;
    }

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

//    ptPos[0] = pos.v[0];
//    ptPos[1] = pos.v[1];
//    ptPos[2] = pos.v[2];

    ptPos[0] = (rand() % 256) - 128;
    ptPos[1] = (rand() % 256) - 128;
    ptPos[2] = (rand() % 256) - 128;

    ptColor[3] = rand() % 8;
    ptColor[0] = 25 + (rand() % 230);
    ptColor[1] = 25 + (rand() % 230);
    ptColor[2] = 25 + (rand() % 230);
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

  joypad_init();
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  t3d_init((T3DInitParams){});
  tpx_init((TPXInitParams){});

  T3DModel *model = t3d_model_load("rom://scene.t3dm");
  rspq_block_begin();
    t3d_model_draw(model);
  model->userBlock = rspq_block_end();

  T3DMat4FP *matFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DVec3 camPos = {{3.0f, 65.0f, 65.0f}};
  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};
  float camRotX = 1.5;
  float camRotY = 4.0f;

  int particleCount = 164;
  TPXParticle *particles = malloc_uncached(sizeof(TPXParticle) * particleCount/2);
  generateParticles(particles, particleCount, true);

  uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0xFF};
  T3DVec3 lightDirVec = {{0.0f, 0.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  T3DViewport viewport = t3d_viewport_create();
  float rotAngle = 0.0f;

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

    {
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

    // we can set up our viewport settings beforehand here
    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(80.0f), 5.0f, 200.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf()); // set the target to draw to
    t3d_frame_start(); // call this once per frame at the beginning of your draw function

    t3d_viewport_attach(&viewport); // now use the viewport, this applies proj/view matrices and sets scissoring

    t3d_screen_clear_color(RGBA32(30, 30, 30, 0));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient); // one global ambient light, always active
    t3d_light_set_count(0);

    float scale = 0.5f;
    t3d_mat4fp_from_srt_euler(matFP,
      (float[]){scale, scale, scale},
      (float[]){rotAngle,rotAngle*2.4f,rotAngle*1.3f},
      (float[]){0,0,0}
    );
    t3d_matrix_push(matFP);

    rspq_block_run(model->userBlock);

    rdpq_set_mode_standard();
    rdpq_mode_zbuf(true, true);
    rdpq_mode_zoverride(true, 0, 0);
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);

    tpx_state_from_t3d(); // copy the current state from t3d to tpx

    //uint64_t ticks = get_ticks();
    tpx_particle_draw(particles, particleCount);
    //rspq_wait();
    //ticks = get_ticks() - ticks;

    t3d_matrix_pop(1);

    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 24, 24, "Cam: %.2f %.2f %.2f", camPos.v[0], camPos.v[1], camPos.v[2]);
    //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 24, 24+12, "RSP: %.2f", TICKS_TO_US(ticks) / 1000.0f);

    rdpq_detach_show();
  }
}

