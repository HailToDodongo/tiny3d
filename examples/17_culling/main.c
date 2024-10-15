#include <libdragon.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

typedef struct
{
  rspq_block_t *dpl;
  T3DVec3 aabbMin;
  T3DVec3 aabbMax;
  int triCount;
  bool visible;
} CullObject;

/**
 * Example showcasing an implementation of frustum culling,
 * using a few builtin helper functions in t3d.
 */
 [[noreturn]]
int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  t3d_debug_print_init();
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();

  // Textures (CC0): https://opengameart.org/content/16x16-block-texture-set
  // Model (CC BY 4.0, modified): https://sketchfab.com/3d-models/a-minecraft-world-ee753675653240eeb71c7b2b8bf95ffe
  T3DModel *model = t3d_model_load("rom://scene.t3dm");
  //T3DModel *model = t3d_model_load("rom://grid.t3dm");

  float modelScale = 0.5f;
  T3DMat4FP* modelMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
  t3d_mat4fp_from_srt_euler(modelMatFP,
    (float[]){modelScale,modelScale,modelScale},
    (float[]){0,0,0},
    (float[]){0,0,0}
  );

  T3DVec3 camPos = {{2.9232f, 37.6248f, 31.1093f}};

  T3DVec3 camTarget = {{0,0,0}};
  T3DVec3 camDir = {{0,0,1}};
  T3DVec3 camPosScreen;
  T3DVec3 camTargetScreen;

  float camRotX = 1.0f;
  float camRotY = 0.0f;

  float rotAngle = 0.0f;
  T3DVec3 rotAxis = {{1.0f, 2.5f, 0.25f}};
  t3d_vec3_norm(&rotAxis);

  double lastTimeMs = 0;
  float time = 0.0f;
  bool debugView = false;
  bool isNight = false;

  // Prepare culling. How exactly this is done may depend on your game in the end.
  // For this example we are reading out the included AABB data and transform it into world space.
  // since it is a static map, there are no further or dynamic transformations.
  // In addition, we also record each object, to directly draw it later.
  uint32_t objCount = t3d_model_get_chunk_count(model, T3D_CHUNK_TYPE_OBJECT);
  CullObject *cullObjs = (CullObject*)malloc_uncached(sizeof(CullObject) * objCount);

  auto it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);
  int idx = 0;
  while(t3d_model_iter_next(&it))
  {
    T3DObject *obj = it.object;

    // record model, we pass NULL as the state arg to be able to draw each object isolated
    // if you want less state changes you can pass a state into it, however you have to make sure
    // to not cull any object that may partially change materials then.
    rspq_block_begin();
      t3d_model_draw_material(obj->material, NULL);
      t3d_model_draw_object(obj, NULL);
    cullObjs[idx].dpl = rspq_block_end();

    // extract AABB and scale to world-space
    t3d_vec3_scale(&cullObjs[idx].aabbMin, &(T3DVec3){{obj->aabbMin[0], obj->aabbMin[1], obj->aabbMin[2]}}, modelScale);
    t3d_vec3_scale(&cullObjs[idx].aabbMax, &(T3DVec3){{obj->aabbMax[0], obj->aabbMax[1], obj->aabbMax[2]}}, modelScale);

    cullObjs[idx].triCount = it.object->triCount;
    ++idx;
  }

  for(;;)
  {
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;
    if(pressed.a || pressed.b)debugView = !debugView;
    if(pressed.start)isNight = !isNight;

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

    rotAngle += 0.03f;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(75.0f), 0.5f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // update visibility, we can use 't3d_frustum_vs_aabb' to check if an object is visible
    // inside the current view frustum set by 't3d_viewport_look_at'
    uint64_t ticksStart = get_ticks();
    for(int i=0; i<objCount; ++i)
    {
      CullObject *obj = &cullObjs[i];
      obj->visible = t3d_frustum_vs_aabb(&viewport.viewFrustum, &obj->aabbMin, &obj->aabbMax);
    }
    ticksStart = get_ticks() - ticksStart;

    if(debugView) {
      T3DVec3 camPosDebug = (T3DVec3){{camTarget.v[0]+2, 220.0f, camTarget.v[2]}};
      t3d_viewport_look_at(&viewport, &camPosDebug, &camTarget, &(T3DVec3){{0,1,0}});
      t3d_viewport_calc_viewspace_pos(&viewport, &camPosScreen, &camPos);

      t3d_vec3_add(&camTarget, &camPos, &(T3DVec3){{camDir.v[0]*100, camDir.v[1]*100, camDir.v[2]*100}});
      t3d_viewport_calc_viewspace_pos(&viewport, &camTargetScreen, &camTarget);
    }

    // ----------- DRAW ------------ //
    rdpq_attach(display_get(), display_get_zbuf());

    t3d_frame_start();
    rdpq_mode_antialias(AA_REDUCED);
    t3d_viewport_attach(&viewport);

    rdpq_set_prim_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    if(isNight) {
      rdpq_set_fog_color(RGBA32(11, 11, 11, 0xFF));
      t3d_screen_clear_color(RGBA32(11, 11, 11, 0xFF));
    } else {
      rdpq_set_fog_color(RGBA32(110, 110, 180, 0xFF));
      t3d_screen_clear_color(RGBA32(110, 110, 180, 0xFF));
    }

    t3d_screen_clear_depth();

    t3d_fog_set_range(37.0f, 180.0f);
    t3d_fog_set_enabled(true);

    if(isNight) {
      t3d_light_set_ambient((uint8_t[]){11, 11, 11, 0xFF});
      t3d_light_set_point(0, (uint8_t[]){0xFF, 0xFF, 0xFF, 0xFF}, &(T3DVec3){{
        camPos.v[0] + camDir.v[0] * 1.0f,
        camPos.v[1] + 1.0f,
        camPos.v[2] + camDir.v[2] * 1.0f
      }}, 0.2f, false);
      t3d_light_set_count(1);
    } else {
      t3d_light_set_ambient((uint8_t[]){0xFF, 0xFF, 0xFF, 0xFF});
    }

    t3d_matrix_push(modelMatFP);

    // Now draw all objects that we determined to be visible
    int triCount = 0;
    for(int i=0; i<objCount; ++i) {
      if(cullObjs[i].visible) {
        rspq_block_run(cullObjs[i].dpl);
        triCount += cullObjs[i].triCount;
      }
    }

    t3d_matrix_pop(1);

    // ----------- DRAW (2D) ------------ //
    t3d_debug_print_start();
    /*rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 20, 20, "FPS: %.2f", display_get_fps());
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 20, 30, "Tris: %d", triCount);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 20, 40, "Cull: %lluus", TICKS_TO_US(ticksStart));

    if(debugView) {
      float len = t3d_vec3_distance(&camPosScreen, &camTargetScreen);

      rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, (int)camPosScreen.v[0]+4, (int)camPosScreen.v[1], "#");
      for(int i=0; i<len; i+=8) {
        float t = (float)i / len;
        rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, (int)(camPosScreen.v[0] + (camTargetScreen.v[0] - camPosScreen.v[0]) * t),
          (int)(camPosScreen.v[1] + (camTargetScreen.v[1] - camPosScreen.v[1]) * t), "+");
      }
    }*/

    t3d_debug_printf(20, 20, "FPS: %.2f", display_get_fps());
    t3d_debug_printf(20, 30, "Tris: %d", triCount);
    t3d_debug_printf(20, 40, "Cull: %lluus", TICKS_TO_US(ticksStart));

    if(debugView) {
      int points = 12;
      T3DVec3 step;
      t3d_vec3_diff(&step, &camTargetScreen, &camPosScreen);
      t3d_vec3_scale(&step, &step, 1.0f / points);

      rdpq_set_mode_fill(RGBA32(0x00, 0x00, 0xFF, 0xFF));
      rdpq_fill_rectangle(camPosScreen.v[0]-4, camPosScreen.v[1]-4, camPosScreen.v[0]+4, camPosScreen.v[1]+4);
      rdpq_set_mode_fill(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));
      for(int i=0; i<points; ++i) {
        rdpq_fill_rectangle(
          (int)(camPosScreen.v[0] + step.v[0] * i) - 1, (int)(camPosScreen.v[1] + step.v[1] * i) - 1,
          (int)(camPosScreen.v[0] + step.v[0] * i) + 1, (int)(camPosScreen.v[1] + step.v[1] * i) + 1
        );
      }
    }


    rdpq_detach_show();
  }
}
