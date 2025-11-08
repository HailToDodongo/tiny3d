#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

/**
 * Example showcasing fresnel.
 * This means getting the angle of triangle/vertex relative to the camera and using it for effects.
 * In t3d, this is done via invisible light-sources that only shine alpha onto a surface.
 * The final scalar is then available as shade-alpha in the color-combiner.
 * Unlike direct light color, this is not multiplied with vertex color so it can be used to brighten up dark areas,
 * as well as being completely independent of any lighting in general.
 *
 * The effect comes in two variants, screen-space and camera-space.
 * The former is done via a directional light, meaning it is very cheap but only works for curved surfaces.
 * A flat plane (e.g. an ocean) would receive the same fresnel across the entire surface if the normal is identical.
 *
 * With a point-light, the effect is calculated per-vertex, meaning it can be used for flat surfaces too.
 * This gives an individual fresnel value per vertex ontop of the normal based on camera-position.
 * That comes at a way higher cost however, so only use it when necessary.
 *
 * The basic usage with light source (like in the '16_light_cull' example) is to have the vertex alpha at 0xFF.
 * Then make all real light sources have an alpha of 0, and only set alpha to 0xFF for the invisible light.
 * Which is pointed directly away from the camera.
 */

#define LIGHT_TYPE_DIR 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SHARP 2

typedef struct {
  T3DModel *model;
  rspq_block_t *dplModel;
  T3DMat4FP *modelMatFP;
  const char* text;
  const char* subText;
  int lightType;
  bool fixedBlend;
  bool disableAA;
  float scale;
} Model;

color_t get_rainbow_color(float s) {
  float r = fm_sinf(s + 0.0f) * 127.0f + 128.0f;
  float g = fm_sinf(s + 2.0f) * 127.0f + 128.0f;
  float b = fm_sinf(s + 4.0f) * 127.0f + 128.0f;
  return RGBA32(r, g, b, 255);
}

#define FB_COUNT 3

[[noreturn]]
int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, FB_COUNT, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  //rdpq_debug_start();
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create_buffered(FB_COUNT);

  T3DVec3 camPos = {{0,10.0f,50.0f}};
  T3DVec3 camTarget = {{0,0,0}};

  uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0x00};

  Model models[] = {
    (Model){.model = t3d_model_load("rom:/sphere.t3dm"), .scale = 0.14f,
      .text = "Fresnel (Screen)",
      .subText = "LERP of tex. and color, screen-space",
      .lightType = LIGHT_TYPE_DIR
    },
    (Model){.model = t3d_model_load("rom:/sphere.t3dm"), .scale = 0.14f,
      .text = "Fresnel (Camera)",
      .subText = "LERP of tex. and color",
      .lightType = LIGHT_TYPE_POINT
    },
    (Model){.model = t3d_model_load("rom:/sphere.t3dm"), .scale = 0.14f,
      .text = "Fresnel 2x",
      .subText = "2 Lights for sharp falloff",
      .lightType = LIGHT_TYPE_SHARP
    },
    (Model){.model = t3d_model_load("rom:/env.t3dm"), .scale = 0.1f,
      .text = "UVGen LERP",
      .subText = "LERP of 2 uvgen textures",
      .lightType = LIGHT_TYPE_POINT
    },
    (Model){.model = t3d_model_load("rom:/force.t3dm"), .scale = 0.18f,
      .text = "Force-Field",
      .subText = "Fresnel to alpha, I8 texture",
      .lightType = LIGHT_TYPE_POINT
    },
    (Model){.model = t3d_model_load("rom:/glass.t3dm"), .scale = 0.18f,
      .text = "Ghost",
      .subText = "Fresnel to alpha, color only",
      .lightType = LIGHT_TYPE_POINT
    },
    (Model){.model = t3d_model_load("rom:/dark.t3dm"), .scale = 0.18f,
      .text = "No-Lighting",
      .subText = "LERP color to env-texture",
      .lightType = LIGHT_TYPE_SHARP
    },
    (Model){.model = t3d_model_load("rom:/clip.t3dm"), .scale = 0.15f,
      .text = "Outline",
      .subText = "Fresnel to alpha-threshold, 2 draws",
      .lightType = LIGHT_TYPE_SHARP,
      .disableAA = true
    },
    (Model){.model = t3d_model_load("rom:/wallColor.t3dm"), .scale = 0.135f,
      .text = "Roughness (Color)",
      .subText = "TEX0/fresnel to alpha, blend with color",
      .lightType = LIGHT_TYPE_SHARP,
      .fixedBlend = true
    },
    (Model){.model = t3d_model_load("rom:/wall.t3dm"), .scale = 0.135f,
      .text = "Roughness (Env)",
      .subText = "2 blended draws, uvgen + no-uvgen",
      .lightType = LIGHT_TYPE_SHARP
    },
  };
  int modelCount = sizeof(models)/sizeof(models[0]);

  for(int i = 0; i < modelCount; i++)
  {
    if(models[i].fixedBlend) { // @TODO: remove once fast64 supports rdpq settings/overrides
      T3DModelIter it = t3d_model_iter_create(models[i].model, T3D_CHUNK_TYPE_MATERIAL);
      while(t3d_model_iter_next(&it)) {
        it.material->blendMode = RDPQ_BLENDER((IN_RGB, IN_ALPHA, BLEND_RGB, INV_MUX_ALPHA));
      }
    }

    models[i].modelMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP) * FB_COUNT);
    rspq_block_begin();
      t3d_model_draw(models[i].model);
    models[i].dplModel = rspq_block_end();
  }

  int currModelIdx = 0;
  float rotAngle = 0.0f;
  T3DVec3 currentPos = {{0,0,0}};
  int frameIdx = 0;

  for(;;)
  {
    // ======== Update ======== //
    frameIdx = (frameIdx + 1) % FB_COUNT;
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(btn.l || btn.c_left || btn.d_left)--currModelIdx;
    if(btn.r || btn.c_right || btn.d_right)++currModelIdx;
    if(currModelIdx < 0)currModelIdx += modelCount;
    currModelIdx %= modelCount;

    Model* model = &models[currModelIdx];

    if(!joypad.btn.z)rotAngle += 0.015f;
    if(joypad.btn.a)rotAngle += 0.04f;
    if(joypad.btn.b)rotAngle = 0;

    T3DVec3 trargetPos = (T3DVec3){{
      joypad.stick_x * 0.4f,
      joypad.stick_y * 0.4f,
      0.0f
    }};
    t3d_vec3_lerp(&currentPos, &currentPos, &trargetPos, 0.2f);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 4.0f, 100.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    t3d_mat4fp_from_srt_euler(&models->modelMatFP[frameIdx],
      (float[3]){model->scale, model->scale, model->scale},
      (float[3]){0.0f, rotAngle*0.8f - 1.5f, rotAngle*0.1f},
      currentPos.v
    );

    color_t colPrim = get_rainbow_color(rotAngle * 0.42f);
    colPrim.a = 0x20;

    // ======== Draw ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    rdpq_mode_antialias(model->disableAA ? AA_NONE : AA_STANDARD);
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color((color_t){40, 40, 60, 0xFF});
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    switch(model->lightType)
    {
      case LIGHT_TYPE_DIR:
        // Approx. using a directional light, only works for curved surfaces but is very fast
        t3d_light_set_directional(0, (uint8_t[4]){0x00, 0x00, 0x00, 0xFF}, &(T3DVec3){{0, 0, 1}});
        t3d_light_set_count(1);
      break;
      case LIGHT_TYPE_POINT:
        // Proper fresnel, but per-vertex, so more expensive
        // Note: point lights exactly at the camera will not work properly, so offset it slightly
        t3d_light_set_point(0,
          (uint8_t[4]){0x00, 0x00, 0x00, 0xFF},
          &(T3DVec3){{camPos.x, camPos.y+0.1f, camPos.z+0.1f}}, 1.0f, false
        );
        t3d_light_set_count(1);
      break;
      case LIGHT_TYPE_SHARP:
        // Same as before but a sharper falloff.
        // This works since each light source is added together, and the CC inverts it
        t3d_light_set_point(0,
          (uint8_t[4]){0x00, 0x00, 0x00, 0xF0},
          &(T3DVec3){{camPos.x+0.075f, camPos.y+0.1f, camPos.z+0.1f}}, 1.0f, false
        );
        t3d_light_set_point(1,
          (uint8_t[4]){0x00, 0x00, 0x00, 0x90},
          &(T3DVec3){{camPos.x+0.075f, camPos.y+0.1f, camPos.z+0.1f}}, 1.0f, false
        );
        t3d_light_set_count(2);
      break;
    }

    rdpq_set_prim_color(colPrim);
    rdpq_set_blend_color(colPrim);

    t3d_matrix_push(&models->modelMatFP[frameIdx]);
      rspq_block_run(model->dplModel);
    t3d_matrix_pop(1);

    // ======== 2D ======== //
    rdpq_sync_pipe();
    rdpq_set_mode_standard();

    // clear coverage + backdrop for text
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY_CONST);
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_change_other_modes_raw(SOM_COVERAGE_DEST_MASK, SOM_COVERAGE_DEST_ZAP);

    rdpq_set_prim_color((color_t){0, 0, 0, 0xFF});
    rdpq_set_fog_color(RGBA32(0, 0, 0, 80));

    uint32_t subTextLen = (strlen(model->subText) * 6 + 8) / 2;
    float centerX = (float)display_get_width() * 0.5f;
    rdpq_fill_rectangle(70, 14, display_get_width()-70, 28);
    rdpq_fill_rectangle(centerX-subTextLen, display_get_height()-34, centerX+subTextLen, display_get_height()-20);

    rdpq_textparms_t texParam = {
        .width = (uint16_t)display_get_width(),
        .align = ALIGN_CENTER,
    };

    rdpq_text_printf(&texParam, FONT_BUILTIN_DEBUG_MONO, 0, 24,
      "< [%d/%d] %s >", currModelIdx+1, modelCount, model->text
    );
    rdpq_text_print(&texParam, FONT_BUILTIN_DEBUG_MONO, 0, display_get_height()-24, model->subText);

    rdpq_detach_show();
  }
}