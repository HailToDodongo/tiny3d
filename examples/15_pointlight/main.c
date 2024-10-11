#include <libdragon.h>
#include <rspq_profile.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

/**
 * Example showcasing point lights.
 * There is nothing too special about them compared to directional lights,
 * except that they have a position and a strength parameter.
 * Slots for directional and point lights are shared so you are limited to 7 lights in total (excl. ambient).
 *
 * Note that point lights are way more expensive than directional lights, so use them sparingly.
 * It's also recommended to have a higher level light system than can use lights per object depending on distance.
 * That way you can also use more than 7 lights in a scene (as long as they never shine on the same object at once).
 *
 * For simplicity, this example just always uses 5 lights without anything fancy.
 */

static float getFloorHeight(const T3DVec3 *pos) {
  // Usually you would have some collision / raycast for this
  // Here we just hardcode the floor height and the one stair
  if(pos->v[2] < -75.0f) {
    float stairScale = 1.0f - (115 + pos->v[2]) / (115.0f-75.0f);
    stairScale = fmaxf(fminf(stairScale, 1.0f), 0.0f);
    return stairScale * 38;
  }
  return 0.0f;
}

typedef struct {
    T3DVec3 pos;
    float strength;
    color_t color;
} PointLight;

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
  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();

  // Credits (CC0): https://kaylousberg.itch.io/kaykit-dungeon-remastered
  T3DModel *model = t3d_model_load("rom://scene.t3dm");

  T3DModel *modelLight = t3d_model_load("rom://light.t3dm");
  T3DModel *modelCursor = t3d_model_load("rom://cursor.t3dm");

  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP* cursorMatFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP* matLightFP = malloc_uncached(sizeof(T3DMat4FP) * 5);

  t3d_mat4fp_from_srt_euler(modelMatFP,
    (float[]){0.15f, 0.15f, 0.15f},
    (float[]){0.0f, 0.0f, 0.0f},
    (float[]){0.0f, 0.0f, 0.0f}
  );

  PointLight pointLights[5] = { // XYZ, strength, color
    {{{ 150.0f, 20.0f,   -5.0f}},  0.055f, {0xFF, 0xFF, 0x1F, 0xFF}},
    {{{-150.0f, 20.0f,   -5.0f}},  0.055f, {0x1F, 0xFF, 0x1F, 0xFF}},
    {{{   0.0f, 40.0f, -190.0f}},  0.300f, {0x1F, 0x1F, 0xFF, 0xFF}},
    {{{   0.0f, 20.0f,   30.0f}},  0.055f, {0xFF, 0x1F, 0x1F, 0xFF}},
    {{{   0.0f, 25.0f,    0.0f}},  0.150f, {0xFF, 0x2F, 0x1F, 0xFF}},
  };

  color_t lightColorOff = {0,0,0, 0xFF};
  uint32_t currLight = 0;
  uint8_t colorAmbient[4] = {20, 20, 20, 0xFF};

  rspq_block_begin();
    t3d_model_draw(model);
  rspq_block_t *dplDraw = rspq_block_end();

  rspq_block_begin();
    t3d_model_draw(modelLight);
  rspq_block_t *dplLight = rspq_block_end();

  rspq_block_begin();
    t3d_model_draw(modelCursor);
  rspq_block_t *dplCursor = rspq_block_end();

  float time = 0.0f;
  float rotAngle = 0.0f;

  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camTargetCurr = {{0,0,0}};
  T3DVec3 camDir = {{1,1,1}};
  float viewZoom = 96.0f;
  bool isOrtho = true;
  bool dirLightOn = false;
  float textTimer = 7.0f;
  rspq_syncpoint_t syncPoint = 0;

  for(;;)
  {
    float deltaTime = display_get_delta_time();
    time += deltaTime;
    rotAngle += deltaTime * 1.25f;

    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    if(pressed.start)isOrtho = !isOrtho;

    // Light selection
    if(pressed.l)currLight--;
    if(pressed.r)currLight++;
    currLight = currLight % 4;
    PointLight *light = &pointLights[currLight];

    // Light controls
    float moveSpeed = deltaTime * 0.75f;
    light->strength += (float)joypad.cstick_x * deltaTime * 0.002f;
    light->strength = fminf(1.0f, fmaxf(0.0f, light->strength));

    light->pos.v[0] -= camDir.v[2] * (float)joypad.stick_x * -moveSpeed;
    light->pos.v[2] += camDir.v[0] * (float)joypad.stick_x * -moveSpeed;
    light->pos.v[0] -= camDir.v[0] * (float)joypad.stick_y * moveSpeed;
    light->pos.v[2] -= camDir.v[2] * (float)joypad.stick_y * moveSpeed;

    light->pos.v[1] += (float)joypad.cstick_y * moveSpeed * -0.4f;

    if(joypad.btn.z)viewZoom += (float)joypad.cstick_x * moveSpeed * 3.5f;
    if(pressed.b)dirLightOn = !dirLightOn;

    // setup camera, look at an isometric angle onto the floor below the selected light
    // and interpolate the camera position to make it smooth
    camTarget = (T3DVec3){{
      light->pos.v[0], getFloorHeight(&light->pos) + 2.0f, light->pos.v[2]
    }};
    t3d_vec3_lerp(&camTargetCurr, &camTargetCurr, &camTarget, 0.2f);

    T3DVec3 camPos;
    t3d_vec3_scale(&camPos, &camDir, 65.0f);
    t3d_vec3_add(&camPos, &camTargetCurr, &camPos);

    if(syncPoint)rspq_syncpoint_wait(syncPoint);

    // flickering and wobble of the crystal in the center of the room
    pointLights[4].pos.v[1] = 24.0f + (sinf(time*2.0f) * 3.5f);
    pointLights[4].strength = 0.2f
       +  (fm_sinf(time * 5.1f) * 0.015f)
       + ((fm_cosf(time * 6.2f)+1) * 0.0025f)
       + ((fm_sinf(time * 2.2f)+0.9f) * 0.01f)
       + ((fm_sinf(time * 1.1f)+0.8f) * 0.01f);

    // cursor below selected point light
    float cursorScale = 0.075f + fm_sinf(rotAngle*6.0f)*0.005f;
    t3d_mat4fp_from_srt_euler(cursorMatFP,
      (float[3]){cursorScale, cursorScale, cursorScale},
      (float[3]){0.0f, rotAngle*0.5f, 0.0f},
      (float[3]){camTarget.v[0], camTarget.v[1] + 0.5f, camTarget.v[2]}
    );

    // update matrix for lights (crystal balls)
    for(int i=0; i<5; ++i)
    {
      float scale = i == currLight ? 0.03f : 0.02f;
      if(i == 4)scale *= 5;

      t3d_mat4fp_from_srt_euler(&matLightFP[i],
        (float[]){scale, scale, scale},
        (float[]){ fm_cosf(time * 2.0f), fm_sinf(time * 2.0f), fm_cosf(time * 2.0f)},
        (float[]){
          pointLights[i].pos.v[0],
          pointLights[i].pos.v[1] + getFloorHeight(&pointLights[i].pos),
          pointLights[i].pos.v[2]
        }
      );
    }

    // projection or ortho/isometric matrix
    float aspect = (float)display_get_height() / (float)display_get_width();
    if(isOrtho) {
      t3d_viewport_set_ortho(&viewport,
        -viewZoom, viewZoom,
        -viewZoom * aspect, viewZoom * aspect,
        3.0f, 250.0f
      );
    } else {
      t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(65.0f), 3.0f, 220.0f);
    }

    t3d_viewport_look_at(&viewport, &camPos, &camTargetCurr, &(T3DVec3){{0,1,0}});

    // ----------- DRAW (3D) ------------ //
    rdpq_attach(display_get(), display_get_zbuf());

    t3d_frame_start();
    rdpq_mode_antialias(AA_NONE);

    t3d_viewport_attach(&viewport);

    rdpq_set_prim_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
    t3d_screen_clear_color(dirLightOn ? RGBA32(40, 40, 60, 0xFF) : RGBA32(20, 20, 30, 0xFF));
    t3d_screen_clear_depth();

    // We can still use an ambient light as usual, this doesn't count towards the 7 light limit
    t3d_light_set_ambient(colorAmbient);

    for(int i=0; i<5; ++i) {
      // Sets the actual point light
      t3d_light_set_point(i, &pointLights[i].color.r, &(T3DVec3){{
        pointLights[i].pos.v[0],
        pointLights[i].pos.v[1] + getFloorHeight(&pointLights[i].pos),
        pointLights[i].pos.v[2]
      }}, pointLights[i].strength, false);
    }
    t3d_light_set_count(5);

    // directional lights can still be used together with point lights
    if(dirLightOn) {
      t3d_light_set_directional(5, (uint8_t[]){0xAA, 0xAA, 0xFF, 0xFF}, &(T3DVec3){{1.0f, 0.0f, 0.0f}});
      t3d_light_set_count(6);
    }

    t3d_matrix_push_pos(1);
    t3d_matrix_set(modelMatFP, true);
    rspq_block_run(dplDraw);

    for(int i=0; i<5; ++i) {
      rdpq_set_prim_color(pointLights[i].strength <= 0.0001f ? lightColorOff : pointLights[i].color);
      t3d_matrix_set(&matLightFP[i], true);
      rspq_block_run(dplLight);
    }

    rdpq_set_prim_color(pointLights[currLight].color);
    t3d_matrix_set(cursorMatFP, true);
    rspq_block_run(dplCursor);

    t3d_matrix_pop(1);
    syncPoint = rspq_syncpoint_new();

    // ----------- DRAW (2D) ------------ //
    if(textTimer > 0.0f) {
      textTimer -= deltaTime;
      float height = (2.0f - textTimer) / 2.0f;
      height = fminf(1.0f, fmaxf(0.0f, height));
      height = (height*height) * -74.0f;

      rdpq_sync_pipe();
      rdpq_sync_tile();

      rdpq_textparms_t txtParam = {.align = ALIGN_CENTER, .width = 320, .wrap = WRAP_WORD};
      rdpq_text_printf(&txtParam, FONT_BUILTIN_DEBUG_MONO, 0, 240 - height - 64, "L/R - Change Light");
      rdpq_text_printf(&txtParam, FONT_BUILTIN_DEBUG_MONO, 0, 240 - height - 48, "C - Height / Strength");
      rdpq_text_printf(&txtParam, FONT_BUILTIN_DEBUG_MONO, 0, 240 - height - 32, "B - Global Light");
      rdpq_text_printf(&txtParam, FONT_BUILTIN_DEBUG_MONO, 0, 240 - height - 16, "START - Camera Mode");
    }

    if(joypad.btn.a) {
      rdpq_sync_pipe();
      rdpq_sync_tile();
      rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 20, 240-20, "FPS: %.2f", display_get_fps());
    }

    rdpq_detach_show();
  }
}
