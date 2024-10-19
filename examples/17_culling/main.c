#include <libdragon.h>

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include "debugDraw.h"

/**
 * Example showcasing an implementation of frustum culling,
 * using a few builtin helper functions in t3d.
 *
 * This will record individual objects in the model, and only draw them if they are visible.
 * Using the BVH functionality for efficient visibility checks.
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

  #define MODEL_COUNT 2
  T3DModel *models[MODEL_COUNT] = {
    // Textures (CC0): https://opengameart.org/content/16x16-block-texture-set
    // Model (CC BY 4.0, modified): https://sketchfab.com/3d-models/a-minecraft-world-ee753675653240eeb71c7b2b8bf95ffe
    t3d_model_load("rom://scene.t3dm"),
    // Credits: Floatland_01" (https://skfb.ly/o8K6t) by eakka
    // licensed under Creative Commons Attribution (http://creativecommons.org/licenses/by/4.0/).
    // Modified by HailToDodongo
    t3d_model_load("rom://platformer.t3dm")
  };

  T3DMat4FP* modelMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

  T3DVec3 camPos = {{-60.0f, 70.0f, 20.f}};
  T3DVec3 camPosTarget = camPos;
  T3DVec3 camDir = {{0,0,1}};
  T3DVec3 camPosScreen, camTargetScreen;

  float camRotX = 0.2f;
  float camRotY = -0.2f;
  float camRotXTarget = camRotX;
  float camRotYTarget = camRotY;
  int currentModel = 0;
  bool debugView = false;
  bool displayBVH = false;
  bool showInfoScreen = true;

  // In order to cull, we must either not record the entire mesh, or do so with individual objects.
  // Here we do the latter. To still take advantage cross-material optimizations, we only record objects
  for(int m=0; m<MODEL_COUNT; ++m) {
    T3DModelIter it = t3d_model_iter_create(models[m], T3D_CHUNK_TYPE_OBJECT);
    while(t3d_model_iter_next(&it)) {
      rspq_block_begin();
      t3d_model_draw_object(it.object, NULL);
      // the object struct offers a 'userBlock' for recording, this is automatically freed when the t3dm object is freed
      // if you need to manually free it, make sure to set it back to NULL afterward
      it.object->userBlock = rspq_block_end();
    }
  }

  uint64_t ticks = 0;
  for(uint32_t frame=1; ; ++frame)
  {
    int visibleObjects = 0, triCount = 0;

    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    if(pressed.b)showInfoScreen = !showInfoScreen;
    if(pressed.a)debugView = !debugView;

    if(pressed.l)currentModel = (currentModel + 1) % MODEL_COUNT;
    if(pressed.r)currentModel = (currentModel + MODEL_COUNT - 1) % MODEL_COUNT;

    if(pressed.start)displayBVH = !displayBVH;
    if(frame > 60 || pressed.a || pressed.b) { ticks = 0; frame = 1; }

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

    if(joypad.btn.c_up)camPosTarget.v[1] += camSpeed * 35.0f;
    if(joypad.btn.c_down)camPosTarget.v[1] -= camSpeed * 35.0f;

    t3d_vec3_lerp(&camPos, &camPos, &camPosTarget, 0.8f);
    camRotX = t3d_lerp(camRotX, camRotXTarget, 0.8f);
    camRotY = t3d_lerp(camRotY, camRotYTarget, 0.8f);
    const T3DModel *model = models[currentModel];

    T3DVec3 camTarget;
    t3d_vec3_add(&camTarget, &camPos, &camDir);

    if(currentModel == 0) {
      t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(75.0f), 1.0f, 160.0f);
    } else {
      t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(60.0f), 4.0f, 110.0f);
    }
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    float modelScale = currentModel == 0 ? 0.5f : 0.15f;
    t3d_mat4fp_from_srt_euler(modelMatFP,
      (float[]){modelScale,modelScale,modelScale},
      (float[]){0,0,0},
      (float[]){0,0,0}
    );

    // since we want to avoid transforming individual AABBs, we transform the frustum
    // to match our map instead (model space). In this case we only have to scale it
    T3DFrustum frustum = viewport.viewFrustum;
    t3d_frustum_scale(&frustum, modelScale);

    // before we do any finer checks with BVH, we test if the entire model is visible
    // note that this data is always available in all models, even without a BVH
    bool modelIsVisible = t3d_frustum_vs_aabb_s16(&frustum, model->aabbMin, model->aabbMax);

    uint64_t ticksStart = get_ticks();
    if(modelIsVisible) {
      // If visible, perform more detailed checks with the BVH (if present in the file)
      // you can also iterate over all AABBs directly (always present) and perform a manual frustum check
      // Note that it might be worth measuring the performance difference between the two methods
      // since at lower object counts the BVH might not be as efficient as a simple linear check

      const T3DBvh *bvh = t3d_model_bvh_get(model); // BVHs are optional, use '--bvh' in the gltf importer (see Makefile)
      if(bvh) {
        t3d_model_bvh_query_frustum(bvh, &frustum);
      } else {
        // without BVH, you can still iterate over all objects and perform a manual frustum checks
        T3DModelIter it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);
        while(t3d_model_iter_next(&it)) {
          it.object->isVisible = t3d_frustum_vs_aabb_s16(&frustum, it.object->aabbMin, it.object->aabbMax);
        }
      }
    }

    ticks += get_ticks() - ticksStart;

    // Debug top-down view, since visibility was already calculated
    // we can switch the camera to view the mesh independently of culling
    if(debugView) {
      T3DVec3 camPosDebug = (T3DVec3){{camTarget.v[0]+2, currentModel == 0 ? 225.0f : 130.0f, camTarget.v[2]}};
      t3d_viewport_look_at(&viewport, &camPosDebug, &camTarget, &(T3DVec3){{0,1,0}});
      t3d_viewport_calc_viewspace_pos(&viewport, &camPosScreen, &camPos);

      t3d_vec3_add(&camTarget, &camPos, &(T3DVec3){{camDir.v[0]*100, camDir.v[1]*100, camDir.v[2]*100}});
      t3d_viewport_calc_viewspace_pos(&viewport, &camTargetScreen, &camTarget);
    }

    // ----------- DRAW ------------ //
    surface_t *surface = display_get();
    rdpq_attach(surface, display_get_zbuf());

    t3d_frame_start();
    rdpq_mode_antialias(AA_REDUCED);
    t3d_viewport_attach(&viewport);

    rdpq_set_prim_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color(RGBA32(110, 110, 210, 0xFF));
    t3d_screen_clear_color(RGBA32(110, 110, 210, 0xFF));

    t3d_screen_clear_depth();

    t3d_fog_set_range(37.0f, 180.0f);
    t3d_fog_set_enabled(true);

    t3d_light_set_ambient((uint8_t[]){0xFF, 0xFF, 0xFF, 0xFF});
    t3d_light_set_count(0);

    t3d_matrix_push(modelMatFP);

    // Now draw all objects that we determined to be visible
    // we still want to optimize materials, so we create a state here and draw them directly
    // the objects (so vertex loads + triangle draws) are recorded since they don't depend on visibility
    int totalObjects = 0;
    if(modelIsVisible)
    {
      T3DModelState state = t3d_model_state_create();
      T3DModelIter it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);
      while(t3d_model_iter_next(&it)) {
        if(it.object->isVisible) {
          // draw material and object
          t3d_model_draw_material(it.object->material, &state);
          rspq_block_run(it.object->userBlock);
          it.object->isVisible = false; // BVH only sets visible objects, so we need to reset this

          // collect some metrics
          ++visibleObjects;
          triCount += it.object->triCount;
        }
        ++totalObjects;
      }
    }

    t3d_matrix_pop(1);

    // ----------- DRAW (2D) ------------ //
    t3d_debug_print_start();
    if(!displayBVH) {
      t3d_debug_printf(18, 18, "Tris: %d", triCount);
      t3d_debug_printf(320-96, 18, "%.2f FPS", display_get_fps());
    }
    t3d_debug_printf(18, 240-24, "BVH: %lluus (%d/%d)", TICKS_TO_US(ticks / frame), visibleObjects, totalObjects);

    if(showInfoScreen) {
      const char* INFO[] = {
        "A: Toggle debug view", "Start: Toggle BVH debug",
        "Z + Stick: Rotate", "Stick: Move",
        "C-U/D: Move up/down", "L/R: change model",
        " ( Press B to close )"
      };
      for(int i=0; i<7; ++i) {
        t3d_debug_printf(74, 70+(i*14), INFO[i]);
      }
    }

    // Top-down debug view, this shows the camera position and direction
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

    if (displayBVH) {
      // Debug draw, this visualizes the BVH tree
      rdpq_detach_wait();
      uint16_t *buff = (uint16_t*)surface->buffer;
      debugDrawBVTree(buff, t3d_model_bvh_get(model), &viewport, &frustum, modelScale);
      display_show(surface);
    } else {
      rdpq_detach_show();
    }
  }
}

