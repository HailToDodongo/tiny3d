#include <libdragon.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

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

  T3DVec3 camPos = {{-60.0f, 70.0f, 20.f}};
  T3DVec3 camPosTarget = camPos;
  T3DVec3 camDir = {{0,0,1}};
  T3DVec3 camPosScreen, camTargetScreen;

  float camRotX = -1.2f;
  float camRotY = -0.2f;
  float camRotXTarget = camRotX;
  float camRotYTarget = camRotY;
  bool debugView = false;
  bool useBVH = true;
  bool useBlock = true;

  uint32_t objCount = t3d_model_get_chunk_count(model, T3D_CHUNK_TYPE_OBJECT);
  T3DModelIter it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);
  while(t3d_model_iter_next(&it)) {
    rspq_block_begin();
    t3d_model_draw_object(it.object, NULL);
    it.object->userBlock = rspq_block_end();
  }

  uint64_t ticks = 0;
  uint64_t ticksBlock = 0;
  for(uint32_t frame=1; ; ++frame)
  {
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;
    if(pressed.start)debugView = !debugView;
    if(pressed.a)useBVH = !useBVH;
    if(pressed.b)useBlock = !useBlock;
    if(frame > 60 || pressed.a || pressed.b) { ticksBlock = 0; ticks = 0; frame = 1; }

    // Camera controls:
    float camSpeed = display_get_delta_time() * 1.1f;
    float camRotSpeed = display_get_delta_time() * 0.025f;

    camDir.v[0] = fm_cosf(camRotX) * fm_cosf(camRotY);
    camDir.v[1] = fm_sinf(camRotY);
    camDir.v[2] = fm_sinf(camRotX) * fm_cosf(camRotY);
    t3d_vec3_norm(&camDir);

    if(joypad.btn.z) {
      camRotXTarget += (float)joypad.stick_x * camRotSpeed;
      camRotYTarget -= (float)joypad.stick_y * camRotSpeed;
    } else {
      camPosTarget.v[0] += camDir.v[0] * (float)joypad.stick_y * camSpeed;
      camPosTarget.v[1] += camDir.v[1] * (float)joypad.stick_y * camSpeed;
      camPosTarget.v[2] += camDir.v[2] * (float)joypad.stick_y * camSpeed;

      camPosTarget.v[0] += camDir.v[2] * (float)joypad.stick_x * -camSpeed;
      camPosTarget.v[2] -= camDir.v[0] * (float)joypad.stick_x * -camSpeed;
    }

    if(joypad.btn.c_up)camPosTarget.v[1] += camSpeed * 15.0f;
    if(joypad.btn.c_down)camPosTarget.v[1] -= camSpeed * 15.0f;

    t3d_vec3_lerp(&camPos, &camPos, &camPosTarget, 0.8f);
    camRotX = t3d_lerp(camRotX, camRotXTarget, 0.8f);
    camRotY = t3d_lerp(camRotY, camRotYTarget, 0.8f);

    T3DVec3 camTarget;
    t3d_vec3_add(&camTarget, &camPos, &camDir);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(75.0f), 0.5f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    T3DFrustum frustum = viewport.viewFrustum;
    // scale frustum to model space (0.5)
    for(int i=0; i<6; ++i) {
      frustum.planes[i].v[0] *= modelScale;
      frustum.planes[i].v[1] *= modelScale;
      frustum.planes[i].v[2] *= modelScale;
    }

    // update visibility, we can use 't3d_frustum_vs_aabb' to check if an object is visible.
    // 't3d_viewport_look_at' will update this automatically
    const T3DBvh *bvh = t3d_model_bvh_get(model);
    t3d_model_bvh_query_frustum(bvh, &frustum);
    assert(bvh); // BHVs are optional, you have to use '--bhv' in the gltf importer (see Makefile)

    int visibleCount = 0, triCount = 0;
    uint64_t ticksStart = get_ticks();
    if(useBVH) {
      t3d_model_bvh_query_frustum(bvh, &frustum);
    } else {
      it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);
      while(t3d_model_iter_next(&it)) {
        it.object->isVisible = t3d_frustum_vs_aabb(&frustum,
          &(T3DVec3){{ it.object->aabbMin[0], it.object->aabbMin[1], it.object->aabbMin[2] }},
          &(T3DVec3){{ it.object->aabbMax[0], it.object->aabbMax[1], it.object->aabbMax[2] }}
        );
      }
    }

    ticks += get_ticks() - ticksStart;

    // Debug top-down view, since visibility was already calculated
    // we can switch the camera to view the mesh independently of culling
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
    rdpq_set_fog_color(RGBA32(110, 110, 180, 0xFF));
    t3d_screen_clear_color(RGBA32(110, 110, 180, 0xFF));

    t3d_screen_clear_depth();

    t3d_fog_set_range(37.0f, 180.0f);
    t3d_fog_set_enabled(true);

    t3d_light_set_ambient((uint8_t[]){0xFF, 0xFF, 0xFF, 0xFF});
    t3d_light_set_count(0);

    t3d_matrix_push(modelMatFP);

    // Now draw all objects that we determined to be visible
    // we still want to optimize materials, so we create a state here and draw them directly
    // the objects (so vertex loads + triangle draws) are recorded since they don't depend on visibility
    T3DModelState state = t3d_model_state_create();
    it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);
    while(t3d_model_iter_next(&it)) {
      if(it.object->isVisible) {
        // draw material and object
        t3d_model_draw_material(it.object->material, &state);
        rspq_block_run(it.object->userBlock);
        it.object->isVisible = false; // BVH only sets visible objects, so we need to reset this

        // collect some metrics
        ++visibleCount;
        triCount += it.object->triCount;
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
    t3d_debug_printf(20, 40, "%s: %lluus (%d/%d)", useBVH ? "BVH" : "AABB", TICKS_TO_US(ticks / frame), visibleCount, objCount);
    //t3d_debug_printf(20, 50, "%s: %lluus", useBlock ? "DPL" : "Direct", TICKS_TO_US(ticksBlock));
    t3d_debug_printf(20, 50, "%s", useBlock ? "DPL" : "Direct");

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
