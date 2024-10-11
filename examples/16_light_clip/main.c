#include <libdragon.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

/**
 * Example showcasing alpha-clipping based on lighting.
 * This uses a point-light (ignoring normals) that is completely black, but has an alpha value.
 * Meaning it "shines" an alpha value as a sphere around it.
 * By setting an alpha threshold, you can either cut holes, or reveal geometry wherever light shines.
 * By combining both you can also create a mix between two models.
 */

#define MODE_REVEAL 0
#define MODE_CUTOUT 1
#define MODE_MIXED 2
#define MODE_LIGHT 3

[[noreturn]]
int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  rdpq_init();
  //rdpq_debug_start();

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();

  T3DModel *sceneA = t3d_model_load("rom://sceneA.t3dm");
  T3DModel *sceneB = t3d_model_load("rom://sceneB.t3dm");
  T3DModel *modelLight = t3d_model_load("rom://light.t3dm");

  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP* matLightFP = malloc_uncached(sizeof(T3DMat4FP) * 5);

  t3d_mat4fp_from_srt_euler(modelMatFP,
    (float[]){0.1f, 0.1f, 0.1f},
    (float[]){0.0f, 0.0f, 0.0f},
    (float[]){0.0f, 0.0f, 0.0f}
  );

  uint8_t colorAmbient[4] = {220, 180, 180, 0};
  uint8_t colorAmbientLight[4] = {10, 10, 10, 0xFF};

  rspq_block_begin();
    t3d_model_draw(sceneA);
  rspq_block_t *dplSceneA = rspq_block_end();

  rspq_block_begin();
    t3d_model_draw(sceneB);
  rspq_block_t *dplSceneB = rspq_block_end();

  rspq_block_begin();
    t3d_model_draw(modelLight);
  rspq_block_t *dplLight = rspq_block_end();

  float time = 0.0f;
  float rotAngle = 0.0f;

  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camPos = camTarget;
  T3DVec3 camPosCurrent = camPos;
  T3DVec3 lightPos = {{0.0f, 0.0f, 0.0f}};

  float angleHor, angleVer;
  float camDist = 130.0f;
  int currMode = MODE_MIXED;
  bool ignoreNormals = true;
  int ditherMode = 0;

  rspq_syncpoint_t syncPoint = 0;

  for(;;)
  {
    float deltaTime = display_get_delta_time();

    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    if(pressed.c_right || pressed.d_right)currMode = (currMode + 1) % 4;
    if(pressed.c_left || pressed.d_left)currMode = (currMode + 3) % 4;
    if(pressed.a)ignoreNormals = !ignoreNormals;
    if(pressed.b)ditherMode = (ditherMode + 1) % 3;

    if(!joypad.btn.z) {
      time += deltaTime;
      rotAngle += deltaTime * 1.25f;
    }

    lightPos.v[0] = fm_sinf(time * 0.8f) * 44.0f;
    lightPos.v[1] = fm_sinf(-time * 2.4f) * 15.0f + 24.0f;
    lightPos.v[2] = fm_cosf(time * 0.8f) * 44.0f;

    t3d_mat4fp_from_srt_euler(matLightFP,
      (float[]){0.05f, 0.05f, 0.05f},
      (float[]){0.0f, rotAngle, 0.0f},
      (float[]){lightPos.v[0], lightPos.v[1], lightPos.v[2]}
    );

    if(syncPoint)rspq_syncpoint_wait(syncPoint);

    camDist += joypad.cstick_y * 0.01f;
    angleHor = T3D_DEG_TO_RAD(60.0f) - joypad.stick_x * 0.0085f;
    angleVer = T3D_DEG_TO_RAD(45.0f) + joypad.stick_y * 0.0085f;
    camPos.v[0] = fm_cosf(angleHor) * fm_cosf(angleVer) * camDist;
    camPos.v[1] = fm_sinf(angleVer) * camDist;
    camPos.v[2] = fm_sinf(angleHor) * fm_cosf(angleVer) * camDist;
    t3d_vec3_lerp(&camPosCurrent, &camPosCurrent, &camPos, deltaTime * 5.0f);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(64.0f), 2.0f, 200.0f);
    t3d_viewport_look_at(&viewport, &camPosCurrent, &camTarget, &(T3DVec3){{0,1,0}});

    // ----------- DRAW (3D) ------------ //
    rdpq_attach(display_get(), display_get_zbuf());

    t3d_frame_start();
    rdpq_mode_antialias(AA_NONE);
    if(ditherMode == 0)rdpq_mode_dithering(DITHER_NONE_NONE);
    if(ditherMode == 1)rdpq_mode_dithering(DITHER_SQUARE_SQUARE);
    if(ditherMode == 2)rdpq_mode_dithering(DITHER_NOISE_NOISE);

    rdpq_set_env_color((color_t){0, 0, 0, 0});

    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(20, 20, 30, 0xFF));
    t3d_screen_clear_depth();
    t3d_fog_set_enabled(false);

    t3d_light_set_directional(0, (uint8_t[]){0x99, 0x99, 0xAA, 0}, &(T3DVec3){{0.0f, 1.0f, 0.0f}});

    if(currMode == MODE_LIGHT) {
      t3d_light_set_ambient(colorAmbientLight);
      t3d_light_set_point(1, (uint8_t[]){0xFF, 0xFF, 0xFF, 0xFF}, &lightPos, 0.125f, ignoreNormals);
    } else {
      // cutout lighting, this works by using the alpha channel of the light color
      // by setting the ambient alpha to 0, only dir/point lights will affect the scene
      // this can be done completely independent of the actual color values
      t3d_light_set_ambient(colorAmbient);
      t3d_light_set_point(1, (uint8_t[]){0, 0, 0, 0xFF}, &lightPos, 0.125f, ignoreNormals);
    }
    t3d_light_set_count(2);

    t3d_matrix_push_pos(1);
    t3d_matrix_set(modelMatFP, true);

    if(currMode != MODE_CUTOUT)rspq_block_run(dplSceneA);
    if(currMode == MODE_CUTOUT || currMode == MODE_MIXED)rspq_block_run(dplSceneB);

    t3d_matrix_set(matLightFP, true);
    rspq_block_run(dplLight);

    t3d_matrix_pop(1);
    syncPoint = rspq_syncpoint_new();

    // ----------- DRAW (2D) ------------ //
    rdpq_sync_pipe();
    rdpq_sync_tile();
    const char* MODES[] = {"Reveal", "Cutout", "Mixed", "Light"};
    const char* DITHER[] = {"None", "Square", "Noise"};

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 20, 240 - 18,    "[C-LR] Mode: %s", MODES[currMode]);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 20, 240 - 18-12, "[A] Sphere: %d", ignoreNormals ? 1 : 0);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 20, 240 - 18-24, "[B] Dither: %s", DITHER[ditherMode]);

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 20, 24, "FPS: %.2f", display_get_fps());

    rdpq_detach_show();
  }
}
