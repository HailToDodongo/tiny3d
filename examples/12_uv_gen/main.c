#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

/**
 * Example showcases various effects that can be done with generated (spherical) UVs.
 * It will generate UVs on the fly based on the normal vector to map onto a sphere-texture.
 * This is useful for environment-mapping, or reflective, shiny & metallic surfaces.
 *
 * Most of the magic happens in the blender files material settings, the C code here
 * is mostly stuff that was shown in previous examples.
 */

typedef struct {
  T3DModel *model;
  rspq_block_t *dplModel;
  T3DMat4FP *modelMatFP;
  const char* text;
  const char* subText;
  float scale;
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

  uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0xFF};

  Model models[] = {
    (Model){.model = t3d_model_load("rom:/sphere.t3dm"), .scale = 0.13f,
      .text = "Smooth Shapes",
      .subText = "Basic uv-gen. with 44x44 texture"
    },
    (Model){.model = t3d_model_load("rom:/teapot.t3dm"), .scale = 0.08f,
      .text = "Teapot",
      .subText = "Rough-Metallic (blurred env. texture)"
    },
    (Model){.model = t3d_model_load("rom:/gold.t3dm"), .scale = 0.03f,
      .text = "Golden Egg",
      .subText = "LERP between two env. textures"
    },
    (Model){.model = t3d_model_load("rom:/can.t3dm"), .scale = 0.13f,
      .text = "Soda-Can",
      .subText = "Alpha-blend, two meshes (normal & uv-gen)"
    },
    (Model){.model = t3d_model_load("rom:/color.t3dm"), .scale = 0.15f,
      .text = "Colored Cube",
      .subText = "LERP between vert. color and env. texture"
    },
    (Model){.model = t3d_model_load("rom:/flat.t3dm"), .scale = 0.05f,
      .text = "Flat Shapes",
      .subText = "Flat-shaded mesh, smooth uv-gen."
    },
    (Model){.model = t3d_model_load("rom:/gem.t3dm"), .scale = 0.05f,
      .text = "Gem Stone",
      .subText = "Flat-shaded mesh, 4x4 env. texture"
    },
    (Model){.model = t3d_model_load("rom:/brew_logo.t3dm"), .scale = 0.07f,
      .text = "N64Brew Logo",
      .subText = "Various env. textures (Model by: Arthur)"
    },
  };
  uint32_t modelCount = sizeof(models)/sizeof(models[0]);

  for(int i = 0; i < modelCount; i++) {
    models[i].modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
    rspq_block_begin();
      t3d_matrix_push(models[i].modelMatFP);
      t3d_model_draw(models[i].model);
      t3d_matrix_pop(1);
    models[i].dplModel = rspq_block_end();
  }

  uint32_t currModelIdx = 0;
  float rotAngle = 0.0f;
  rspq_syncpoint_t syncPoint = 0;
  T3DVec3 currentPos = {{0,0,0}};

  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    uint32_t lastModelIdx = currModelIdx;
    if(btn.l || btn.c_left || btn.d_left)--currModelIdx;
    if(btn.r || btn.c_right || btn.d_right)++currModelIdx;
    currModelIdx %= modelCount;
    if(lastModelIdx != currModelIdx)rotAngle = 0.0f;

    Model* model = &models[currModelIdx];

    rotAngle += 0.015f;
    if(joypad.btn.a)rotAngle += 0.04f;
    if(joypad.btn.b)rotAngle = 0;

    T3DVec3 trargetPos = (T3DVec3){{
      joypad.stick_x * 0.4f,
      joypad.stick_y * 0.4f,
      0.0f
    }};
    t3d_vec3_lerp(&currentPos, &currentPos, &trargetPos, 0.2f);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 4.0f, 160.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    if(syncPoint)rspq_syncpoint_wait(syncPoint);
    t3d_mat4fp_from_srt_euler(model->modelMatFP,
      (float[3]){model->scale, model->scale, model->scale},
      (float[3]){0.0f, rotAngle*0.8f - 1.5f, rotAngle*0.1f},
      currentPos.v
    );

    // ======== Draw ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(217, 174, 147, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(0);

    rdpq_set_prim_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
    rspq_block_run(model->dplModel);
    syncPoint = rspq_syncpoint_new();

    // ======== 2D ======== //
    rdpq_sync_pipe();

    rdpq_textparms_t texParam = {
        .width = display_get_width(),
        .align = ALIGN_CENTER,
    };

    rdpq_text_printf(&texParam, FONT_BUILTIN_DEBUG_MONO, 0, 24,
      "< [%ld/%ld] %s >", currModelIdx+1, modelCount, model->text
    );
    rdpq_text_print(&texParam, FONT_BUILTIN_DEBUG_MONO, 0, display_get_height()-24, model->subText);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}
