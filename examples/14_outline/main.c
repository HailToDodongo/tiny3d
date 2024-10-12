#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

/**
 * Showcase for outlines.
 *
 * By drawing an inverted and scaled (by normals) version of a model in black, you can create an outline effect.
 * To avoid manually scaling a model, you can use the T3D_VERTEX_FX_OUTLINE vertex function.
 * This will scale the triangles with a constant offset in screen-space based on the vertex normal.
 * In contrast to the manual approach, this will create a constant outline (in pixel) independent of distance and perspective.
 *
 * This effect only properly works with smooth-shaded triangles, if you have a flat-shaded one
 * you have to create a second mesh which has the same scale as the base model.
 * For the best results make sure the mesh has no hard edges.
 *
 * Note that this effect is currently not working when clipped, so make sure your objects are small enough to fit within the guard-band.
 */

color_t get_rainbow_color(float s, float brightness) {
  float r = fm_sinf(s) * 0.5f + 0.5f;
  float g = fm_sinf(s + 2.094f) * 0.5f + 0.5f;
  float b = fm_sinf(s + 4.188f) * 0.5f + 0.5f;
  return (color_t){
    (uint8_t)(r * brightness * 255.0f),
    (uint8_t)(g * brightness * 255.0f),
    (uint8_t)(b * brightness * 255.0f),
    0xFF
  };
}

#define FONT_MAIN 2

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();

  rdpq_font_t *fnt = rdpq_font_load("rom:/fibberish.font64");
  rdpq_font_style(fnt, 0, &(rdpq_fontstyle_t){.color = (color_t){0xFF, 0xFF, 0xFF, 0xFF}});
  rdpq_font_style(fnt, 1, &(rdpq_fontstyle_t){.color = (color_t){232, 101, 65, 0xFF}});
  rdpq_font_style(fnt, 2, &(rdpq_fontstyle_t){.color = (color_t){79, 209, 133, 0xFF}});
  rdpq_text_register_font(FONT_MAIN, fnt);

  sprite_t *spriteBox = sprite_load("rom:/textbox.i8.sprite");

  joypad_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();

  T3DVec3 camPos = {{0,10.0f,50.0f}};
  T3DVec3 camTarget = {{0,0,0}};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 0.0f}};
  T3DVec3 lightDirVec2 = {{1.0f, 1.0f, 0.0f}};
  t3d_vec3_norm(&lightDirVec);
  t3d_vec3_norm(&lightDirVec2);

  // Thew model we want to draw is already smooth-shaded, and only contains a single mesh
  T3DModel *itemModel = t3d_model_load("rom:/potion.t3dm");
  T3DMat4FP *itemMatFP = malloc_uncached(sizeof(T3DMat4FP));

  // Create a block for the regular model here...
  rspq_block_begin();
    t3d_model_draw(itemModel);
  rspq_block_t *itemDpl = rspq_block_end();

  // Then for the outline, in order to get a visual outline effect too we need to invert normals and make it a solid color.
  // The first can be done by reversing the face-culling, the second by using a flat color-combiner.
  rspq_block_begin();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    // In this example we want to dynamically change the outline settings.
    // the code for that is a bit further down. If you don't need this, you can record it here too.
    t3d_state_set_drawflags(T3D_FLAG_CULL_FRONT | T3D_FLAG_DEPTH);

    T3DModelIter it = t3d_model_iter_create(itemModel, T3D_CHUNK_TYPE_OBJECT);
    while(t3d_model_iter_next(&it)) {
      t3d_model_draw_object(it.object, NULL);
    }

    t3d_state_set_vertex_fx(T3D_VERTEX_FX_NONE, 0, 0);
  rspq_block_t *dplOutline = rspq_block_end();

  rspq_block_t *dplTextbox = NULL;

  float rotAngle = 0.0f;
  rspq_syncpoint_t syncPoint = 0;
  T3DVec3 currentPos = {{0,0,0}};

  float colorPos = 0.0f;
  float colorValue = 1.0f;
  float outlineSize = 16.0f;

  T3DVec3 targetPos = (T3DVec3){{0.0f, 7.0f, -4.0f}};

  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);

    rotAngle += 0.015f;
    if(joypad.btn.a)rotAngle += 0.04f;
    if(joypad.btn.b)rotAngle = 0;

    if(joypad.btn.z) {
      targetPos.v[0] = joypad.stick_x * 0.3f;
      targetPos.v[1] = joypad.stick_y * 0.3f + 7.0f;
    } else {
      colorPos += joypad.stick_x * 0.002f;
      colorValue += joypad.stick_y * 0.002f;
      outlineSize += joypad.cstick_x * 0.05f;
      outlineSize = fminf(fmaxf(outlineSize, -0.1f), 150.0f);
    }

    targetPos.v[2] += joypad.cstick_y * 0.1f;
    targetPos.v[2] = fminf(fmaxf(targetPos.v[2], -100.0f), 12.0f);

    t3d_vec3_lerp(&currentPos, &currentPos, &targetPos, 0.2f);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 5.0f, 120.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    if(syncPoint)rspq_syncpoint_wait(syncPoint);

    float scale = 0.13f;
    t3d_mat4fp_from_srt_euler(itemMatFP,
      (float[3]){scale, scale, scale},
      (float[3]){0.0f, rotAngle*0.5f - 1.5f, 0.3f},
      currentPos.v
    );

    // ======== Draw ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(59, 80, 125, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient((uint8_t[4]){0xFF, 0xFF, 0xFF, 0xFF});
    t3d_light_set_count(0);

    t3d_matrix_push(itemMatFP);
      rspq_block_run(itemDpl);

      if(outlineSize > 0.0f) {
        // Before drawing the outline, enable the vertex-effect with given size...
        t3d_state_set_vertex_fx(T3D_VERTEX_FX_OUTLINE, (int16_t)outlineSize, (int16_t)outlineSize);
        // ...and set the color via prim. as set in the CC earlier
        rdpq_set_prim_color(get_rainbow_color(colorPos, fm_sinf(colorValue) * 0.5f + 0.5f));
        rspq_block_run(dplOutline);

        // Note: be sure to draw the outline *after* the actual model, they have the exact same depth-values,
        // and you can rely on the depth-test to filter out most of the model.
      }

    t3d_matrix_pop(1);

    syncPoint = rspq_syncpoint_new();

    // ======== 2D ======== //
    float posCenter = display_get_width() / 2;
    float posY = display_get_height() - 90;
    float bxWidth = 220.0f;
    float bxHeight = 72.0f;
    float posX = posCenter - bxWidth / 2;

    if(!dplTextbox)
    {
      rspq_block_begin();

      rdpq_sync_pipe();
      rdpq_sync_tile();
      rdpq_set_mode_standard();
      rdpq_mode_combiner(RDPQ_COMBINER1((0,0,0,PRIM), (PRIM,0,TEX0,0)));
      rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
      rdpq_set_prim_color((color_t){33, 33, 33, 0x99});

      rdpq_sprite_upload(TILE0, spriteBox, NULL);

      // texture is only the corner, draw 4 times for each corner and extend the clamped texture
      rdpq_texture_rectangle(TILE0, posX,           posY,            posX + bxWidth/2,    posY + bxHeight/2,     0, 0);
      rdpq_texture_rectangle(TILE0, posX,           posY + bxHeight, posX + bxWidth/2,    posY + bxHeight/2 - 1, 0, 0);
      rdpq_texture_rectangle(TILE0, posX + bxWidth, posY,            posX + bxWidth/2 -1, posY + bxHeight/2,     0, 0);
      rdpq_texture_rectangle(TILE0, posX + bxWidth, posY + bxHeight, posX + bxWidth/2 -1, posY + bxHeight/2 - 1, 0, 0);

      // Draw text-box background
      posY += 18;
      // text in the textbox
      rdpq_text_printf(&(rdpq_textparms_t){
        .align = ALIGN_CENTER, .width = bxWidth, .wrap = WRAP_WORD,
      }, FONT_MAIN, posX, posY, "^01~ Magic Potion ~\n");

      rdpq_text_printf(&(rdpq_textparms_t){
        .align = ALIGN_LEFT, .width = bxWidth, .wrap = WRAP_WORD,
        .line_spacing = -4
      }, FONT_MAIN, posX+22, posY + 16,
        "^02[Stick]^00 change color\n"
        "^02[C]^00 outline size & distance\n"
        "^02[Z]^00 hold to change position\n"
      );

      dplTextbox = rspq_block_end();
    }

    rspq_block_run(dplTextbox);

    rdpq_text_printf(NULL, FONT_MAIN, 24, 24, "FPS: %.2f", display_get_fps());
    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}
