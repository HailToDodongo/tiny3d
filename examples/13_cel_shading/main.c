#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

/**
 * Showcase for a cel-shading technique based on generated UVs.
 * This sacrifices usage of textures (by requiring a gradient texture) to get a very fast cel-shading effect.
 * It will write the lights intensity as a UV's x-coordinate to act as a color lookup.
 * The amount of cel-layers can be adjusted by changing the texture and has no effect on performance.
 * If texture are needed, the models must be draw a second time with blending enabled.
 */

typedef struct {
  T3DModel *model;
  rspq_block_t *dplModel;
  rspq_block_t *dplOutline;
  T3DMat4FP *modelMatFP;
  uint8_t colorAmbient[4];
  uint8_t colorDir[4];
  enum T3DVertexFX vertexFx;
  const char* text;
  const char* subText;
  float scale;
  bool hasOutline;
} Model;

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();

  T3DVec3 camPos = {{0,10.0f,50.0f}};
  T3DVec3 camTarget = {{0,0,0}};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 0.0f}};
  T3DVec3 lightDirVec2 = {{1.0f, 1.0f, 0.0f}};
  t3d_vec3_norm(&lightDirVec);
  t3d_vec3_norm(&lightDirVec2);

  T3DModel *arrowModel = t3d_model_load("rom:/arrow.t3dm");
  T3DMat4FP *arrowMatFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP *arrowMat2FP = malloc_uncached(sizeof(T3DMat4FP));

  Model models[] = {
    (Model){.model = t3d_model_load("rom:/pot.t3dm"), .scale = 0.12f,
      .colorAmbient = {0x55, 0x55, 0x55, 0xFF},
      .colorDir = {0xFF, 0xFF, 0xFF, 0xFF},
      .vertexFx = T3D_VERTEX_FX_CELSHADE_COLOR,
      .hasOutline = true,
      .text = "Outlined Pot (Soft)",
      .subText = "1D texture, soft lights (color)\n"
                 "No blending, second draw for outline"
    },
    (Model){.model = t3d_model_load("rom:/pot.t3dm"), .scale = 0.12f,
      .colorAmbient = {0xFF, 0xFF, 0xFF, 0x55},
      .colorDir = {0, 0, 0, 0xFF},
      .vertexFx = T3D_VERTEX_FX_CELSHADE_ALPHA,
      .hasOutline = true,
      .text = "Outlined Pot (Flat)",
      .subText = "1D texture, flat lights (alpha)\n"
                 "No blending, second draw for outline"
    },
    (Model){.model = t3d_model_load("rom:/sphereColor.t3dm"), .scale = 0.12f,
      .colorAmbient = {0x55, 0x55, 0x55, 0x00},
      .colorDir = {0xFF, 0xFF, 0xFF, 0xFF},
      .vertexFx = T3D_VERTEX_FX_CELSHADE_ALPHA,
      .hasOutline = true,
      .text = "Planet",
      .subText = "No color/texture, soft lights\n"
                 "No blending, second draw for outline"
    },
    (Model){.model = t3d_model_load("rom:/monkey.t3dm"), .scale = 0.12f,
      .colorAmbient = {0x33, 0x33, 0x33, 0xFF},
      .colorDir = {0xFF, 0xFF, 0xFF, 0xFF},
      .vertexFx = T3D_VERTEX_FX_CELSHADE_COLOR,
      .hasOutline = true,
      .text = "Suzanne",
      .subText = "vertex color, soft lights\n"
                 "No blending, second draw for outline"
    },
    (Model){.model = t3d_model_load("rom:/potTex.t3dm"), .scale = 0.15f,
      .colorAmbient = {0x33, 0x33, 0x33, 0x00},
      .colorDir = {0xFF, 0xFF, 0xFF, 0xFF},
      .vertexFx = T3D_VERTEX_FX_CELSHADE_ALPHA,
      .text = "Textured Pot",
      .subText = "Regular mesh & cel-shading\n"
                 "Two draws with fixed alpha blend"
    },
    (Model){.model = t3d_model_load("rom:/potTexLight.t3dm"), .scale = 0.15f,
      .colorAmbient = {0x33, 0x33, 0x33, 0x00},
      .colorDir = {0xFF, 0xFF, 0xFF, 0xFF},
      .vertexFx = T3D_VERTEX_FX_CELSHADE_ALPHA,
      .text = "Textured Pot",
      .subText = "Regular mesh & cel-shading\n"
                 "Two draws with intensity alpha blend (light)"
    },
    (Model){.model = t3d_model_load("rom:/potTexDark.t3dm"), .scale = 0.15f,
      .colorAmbient = {0x33, 0x33, 0x33, 0x00},
      .colorDir = {0xFF, 0xFF, 0xFF, 0xFF},
      .vertexFx = T3D_VERTEX_FX_CELSHADE_ALPHA,
      .text = "Textured Pot",
      .subText = "Regular mesh & cel-shading\n"
                 "Two draws with intensity alpha blend (dark)"
    },
    (Model){.model = t3d_model_load("rom:/gold.t3dm"), .scale = 0.12f,
      .colorAmbient = {0x33, 0x33, 0x33, 0x00},
      .colorDir = {0xFF, 0xFF, 0xFF, 0xFF},
      .vertexFx = T3D_VERTEX_FX_CELSHADE_ALPHA,
      .text = "Reflective Sphere",
      .subText = "Spherical-UVs & cel-shading\n"
                 "Two draws with fixed alpha blend"
    },
  };
  uint32_t modelCount = sizeof(models)/sizeof(models[0]);

  for(int i = 0; i < modelCount; i++)
  {
    // fast64 has currently no setting for t3d specific cel-shading, so set it manually here.
    // alternatively, you can also set it from the outside via:
    // t3d_state_set_vertex_fx(T3D_VERTEX_FX_xxx, 0, 0);
    T3DMaterial *celShadeMat = t3d_model_get_material(models[i].model, "celshade");
    if(celShadeMat) {
      celShadeMat->vertexFxFunc = models[i].vertexFx;
    }

    models[i].modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

    rspq_block_begin();
      t3d_model_draw(models[i].model);
    models[i].dplModel = rspq_block_end();

    if(models[i].hasOutline)
    {
      rspq_block_begin();
        rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
        rdpq_set_prim_color((color_t){0, 0, 0, 0xFF});

        // this effect is explained in `14_outline`, but in short:
        // invert model and use a black material to draw the outline, T3D_VERTEX_FX_OUTLINE will grow it slightly
        t3d_state_set_drawflags(T3D_FLAG_CULL_FRONT | T3D_FLAG_DEPTH);
        t3d_state_set_vertex_fx(T3D_VERTEX_FX_OUTLINE, 16, 16);

        T3DModelIter it = t3d_model_iter_create(models[i].model, T3D_CHUNK_TYPE_OBJECT);
        while(t3d_model_iter_next(&it)) {
          t3d_model_draw_object(it.object, NULL);
        }

        t3d_state_set_vertex_fx(T3D_VERTEX_FX_NONE, 0, 0);
      models[i].dplOutline = rspq_block_end();
    }
  }

  rspq_block_begin();
    t3d_matrix_push(arrowMatFP);
    t3d_model_draw(arrowModel);
    t3d_matrix_pop(1);
  rspq_block_t *dplArrow = rspq_block_end();

  rspq_block_begin();
    t3d_matrix_push(arrowMat2FP);
    t3d_model_draw(arrowModel);
    t3d_matrix_pop(1);
  rspq_block_t *dplArrow2 = rspq_block_end();

  uint32_t currModelIdx = 0;
  float rotAngle = 0.0f;
  rspq_syncpoint_t syncPoint = 0;
  T3DVec3 currentPos = {{0,0,0}};
  bool useTwoLights = false;

  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(btn.l || btn.c_left || btn.d_left)--currModelIdx;
    if(btn.r || btn.c_right || btn.d_right)++currModelIdx;
    if(btn.z)useTwoLights = !useTwoLights;
    currModelIdx %= modelCount;

    Model* model = &models[currModelIdx];

    rotAngle += 0.015f;
    if(joypad.btn.a)rotAngle += 0.04f;
    if(joypad.btn.b)rotAngle = 0;

    T3DVec3 targetPos = (T3DVec3){{
      joypad.stick_x * 0.4f,
      joypad.stick_y * 0.4f,
      0.0f
    }};
    t3d_vec3_lerp(&currentPos, &currentPos, &targetPos, 0.2f);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 5.0f, 120.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    if(syncPoint)rspq_syncpoint_wait(syncPoint);

    t3d_mat4fp_from_srt_euler(model->modelMatFP,
      (float[3]){model->scale, model->scale, model->scale},
      (float[3]){0.0f, rotAngle*0.5f - 1.5f, sinf(rotAngle*0.5f)*0.5f},
      currentPos.v
    );

    lightDirVec.v[0] = sinf(rotAngle * 0.5f);
    lightDirVec.v[1] = sinf(rotAngle * 0.3f);
    lightDirVec.v[2] = cosf(rotAngle * 0.5f) + 0.25f;
    t3d_vec3_norm(&lightDirVec);

    lightDirVec2.v[0] = sinf(1.0f + rotAngle * -0.35f);
    lightDirVec2.v[1] = sinf(1.2f + rotAngle * -0.2f);
    lightDirVec2.v[2] = cosf(1.4f + rotAngle * 0.2f) + 0.22f;
    t3d_vec3_norm(&lightDirVec2);

    T3DMat4 rotMat;
    t3d_mat4_rot_from_dir(&rotMat, &lightDirVec, &(T3DVec3){{0,1,0}});
    t3d_mat4_scale(&rotMat, 0.1f, 0.1f, 0.1f);
    t3d_mat4_to_fixed(arrowMatFP, &rotMat);

    t3d_mat4_rot_from_dir(&rotMat, &lightDirVec2, &(T3DVec3){{0,1,0}});
    t3d_mat4_scale(&rotMat, 0.1f, 0.1f, 0.1f);
    t3d_mat4_to_fixed(arrowMat2FP, &rotMat);

    // ======== Draw ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(217, 174, 147, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(model->colorAmbient);
    t3d_light_set_directional(0, model->colorDir, &lightDirVec);

    uint8_t color2[4] = {
      model->colorDir[0],
      0, 0,
      model->colorDir[3]
    };
    t3d_light_set_directional(1, color2, &lightDirVec2);
    t3d_light_set_count(useTwoLights ? 2 : 1);

    rdpq_set_prim_color(*(color_t*)model->colorDir);
    rspq_block_run(dplArrow);
    if(useTwoLights) {
      rdpq_set_prim_color(*(color_t*)color2);
      rspq_block_run(dplArrow2);
    }

    rdpq_set_prim_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});

    t3d_matrix_push(model->modelMatFP);
    rspq_block_run(model->dplModel);
    if(model->dplOutline) {
      rspq_block_run(model->dplOutline);
    }
    t3d_matrix_pop(1);

    syncPoint = rspq_syncpoint_new();

    // ======== 2D ======== //
    rdpq_sync_pipe();

    rdpq_textparms_t texParam = {
        .width = display_get_width(),
        .align = ALIGN_CENTER,
    };

    rdpq_text_printf(&texParam, FONT_BUILTIN_DEBUG_MONO, 0, 28,
      "< [%ld/%ld] %s >", currModelIdx+1, modelCount, model->text
    );
    rdpq_text_print(&texParam, FONT_BUILTIN_DEBUG_MONO, 0, display_get_height()-24, model->subText);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, display_get_width()-70, 16, "FPS: %.2f", display_get_fps());

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}
