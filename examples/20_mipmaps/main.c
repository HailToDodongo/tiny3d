#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

/**
 * Example showcasing mipmaps.
 * This is a hardware feature of the N64 and provided as an API in libdragon itself.
 * A mipmap is a collection of uploaded textures that will be selected based on pixel density.
 * Optionally, those different levels can be blended together.
 * The main use-case is to use smaller versions of a texture when an object is further away.
 * This can reduce aliasing, a performance difference does not exist however.
 * Each level can be freely set, so it can also be used for other effects.
 *
 * Note: For automatic mipmaps, checkout the makefile
 */

typedef struct {
  T3DModel *model;
  rspq_block_t *dplModel;
  T3DMat4FP *modelMatFP;
  const char* text;
  const char* subText;
  float scale;
} Model;

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

  T3DVec3 camPos = {{0,0.0f,50.0f}};
  T3DVec3 camTarget = {{0,0,0}};

  uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0xFF};

  sprite_t *rainbows[6] = {
    sprite_load("rom:/rainbow00.rgba16.sprite"),
    sprite_load("rom:/rainbow01.rgba16.sprite"),
    sprite_load("rom:/rainbow02.rgba16.sprite"),
    sprite_load("rom:/rainbow03.rgba16.sprite"),
    sprite_load("rom:/rainbow04.rgba16.sprite"),
    sprite_load("rom:/rainbow05.rgba16.sprite"),
  };

  Model models[] = {
    (Model){.model = t3d_model_load("rom:/rainbow.t3dm"), .scale = 0.5f,
      .text = "Custom Mipmaps",
      .subText = "6x RGBA16, unique color per level"
    },
    (Model){.model = t3d_model_load("rom:/tunnel.t3dm"), .scale = 0.5f,
      .text = "No Mipmaps",
      .subText = "I8 64x64 + CI4 64x64"
    },
    (Model){.model = t3d_model_load("rom:/tunnelMip.t3dm"), .scale = 0.5f,
      .text = "Auto. Mipmaps",
      .subText = "I8 32x32 + CI4 32x32"
    },
  };
  uint32_t modelCount = sizeof(models)/sizeof(models[0]);

  for(int i = 0; i < modelCount; i++) {
    models[i].modelMatFP = malloc_uncached(sizeof(T3DMat4FP) * FB_COUNT);
    rspq_block_begin();
      if(i == 0) {
        T3DObject *obj = t3d_model_get_object_by_index(models[i].model, 0);

        rdpq_texparms_t texParam = (rdpq_texparms_t){};
        texParam.s.repeats = REPEAT_INFINITE;
        texParam.t.repeats = REPEAT_INFINITE;

        // For manual mipmaps we have to upload the textures individually.
        // This is the same as for multi-texturing, so just advance the tile per texture here.
        rdpq_tex_multi_begin();
          for(int t=0; t<6; ++t) {
            rdpq_sprite_upload(TILE0+t, rainbows[t], &texParam);
          }
        rdpq_tex_multi_end();
        // Lastly, enable mipmaps and set the level to the total amount of textures (incl. the first one)
        // Note: t3d only updates this internally after a state update via 't3d_state_set_drawflags()'.
        // This will happen in 't3d_model_draw_material' however, just be aware of that for any manual usage.
        rdpq_mode_mipmap(MIPMAP_INTERPOLATE, 6);

        t3d_model_draw_material(obj->material, NULL);
        t3d_model_draw_object(obj, NULL);

      } else {
        t3d_model_draw(models[i].model);
      }

    models[i].dplModel = rspq_block_end();
  }

  uint32_t currModelIdx = 0;
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
    currModelIdx %= modelCount;

    Model* model = &models[currModelIdx];

    rotAngle += 0.015f;
    if(joypad.btn.a)rotAngle += 0.04f;
    if(joypad.btn.b)rotAngle = 0;

    T3DVec3 trargetPos = (T3DVec3){{
      joypad.stick_x * 0.4f,
      joypad.stick_y * 0.4f,
      (sinf(rotAngle*0.4f) * 0.5f + 0.5f) * 260.0f
    }};
    t3d_vec3_lerp(&currentPos, &currentPos, &trargetPos, 0.2f);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 14.0f, 240.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    t3d_mat4fp_from_srt_euler(&model->modelMatFP[frameIdx],
      (float[3]){model->scale, model->scale, model->scale},
      (float[3]){0.0f, 0.0f, rotAngle*0.1f},
      currentPos.v
    );

    // ======== Draw ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();

    t3d_viewport_attach(&viewport);

    t3d_screen_clear_depth();
    rdpq_set_prim_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(0);

    t3d_matrix_push(&model->modelMatFP[frameIdx]);
      rspq_block_run(model->dplModel);
    t3d_matrix_pop(1);

    // ======== 2D ======== //
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_set_mode_standard();

    // clear coverage + backdrop for text
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY_CONST);
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_change_other_modes_raw(SOM_COVERAGE_DEST_MASK, SOM_COVERAGE_DEST_ZAP);

    rdpq_set_prim_color((color_t){0, 0, 0, 0xFF});
    rdpq_set_fog_color(RGBA32(0, 0, 0, 80));
    rdpq_fill_rectangle(70, 14, display_get_width()-70, 28);
    rdpq_fill_rectangle(50, display_get_height()-34, display_get_width()-50, display_get_height()-20);

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
}
