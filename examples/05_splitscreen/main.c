/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

#include "collision.h"

#define PLAYER_COUNT 3

typedef struct {
  T3DVec3 position;
  float rot;
  T3DMat4FP* mat;
  color_t color;
} Player;

/**
 * Split-Screen demo.
 * This shows how to use the viewport-API to render to different parts of the screen.
 * Each of the 3 Players has its own viewport and camera.
 */
int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();

  joypad_init();
  t3d_init((T3DInitParams){});

  T3DMat4 tmpMatrix;

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
  t3d_mat4_identity(&tmpMatrix);
  t3d_mat4_scale(&tmpMatrix, 0.1f, 0.1f, 0.1f);
  t3d_mat4_to_fixed(modelMatFP, &tmpMatrix);

  int sizeX = display_get_width();
  int sizeY = display_get_height();

  // Here we allocate multiple viewports to render to different parts of the screen
  // This isn't really any different to other examples, just that we have 3 of them now
  T3DViewport viewports[PLAYER_COUNT] = {t3d_viewport_create(), t3d_viewport_create(), t3d_viewport_create()};
  t3d_viewport_set_area(&viewports[0], 0,       0,       sizeX/2, sizeY/2);
  t3d_viewport_set_area(&viewports[1], sizeX/2, 0,       sizeX/2, sizeY/2);
  t3d_viewport_set_area(&viewports[2], 0,       sizeY/2, sizeX,   sizeY/2-2);

  Player players[PLAYER_COUNT] = {
    {{{-50, 0, 50}}, 0, malloc_uncached(sizeof(T3DMat4FP)), RGBA32(220, 100, 100, 0xFF)},
    {{{ 50, 0, 50}}, 0, malloc_uncached(sizeof(T3DMat4FP)), RGBA32(100, 200, 100, 0xFF)},
    {{{ 50, 0,-50}}, 0, malloc_uncached(sizeof(T3DMat4FP)), RGBA32(100, 100, 200, 0xFF)},
  };

  uint8_t colorAmbient[4] = {250, 220, 220, 0xFF};

  T3DModel *model = t3d_model_load("rom:/model.t3dm");
  rspq_block_begin();
  t3d_model_draw(model);
  rspq_block_t *dplMap = rspq_block_end();

  T3DModel *modelPlayer = t3d_model_load("rom:/cube.t3dm");
  rspq_block_begin();
  t3d_model_draw(modelPlayer);
  rspq_block_t *dplPlayer = rspq_block_end();

  sprite_t *spriteMinimap = sprite_load("rom:/minimap.ia4.sprite");
  sprite_t *spritePlayer = sprite_load("rom:/playerIcon.i4.sprite");

  float playerRot = 0.0f;
  int selPlayer = 0;
  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    if(btn.l)--selPlayer;
    if(btn.r)++selPlayer;
    if(selPlayer < 0)selPlayer = 2;
    if(selPlayer > 2)selPlayer = 0;
    playerRot += 0.05f;

    // Player movement
    players[selPlayer].rot += joypad.stick_x * 0.0007f;
    T3DVec3 moveDir = {{
        fm_cosf(players[selPlayer].rot) * (joypad.stick_y * 0.006f), 0.0f,
        fm_sinf(players[selPlayer].rot) * (joypad.stick_y * 0.006f)
    }};

    t3d_vec3_add(&players[selPlayer].position, &players[selPlayer].position, &moveDir);
    check_map_collision(&players[selPlayer].position);

    for(int p=0; p<PLAYER_COUNT; ++p) {
      t3d_mat4fp_from_srt_euler(players[p].mat,
        (float [3]){0.06f, 0.06f + fm_sinf(playerRot) * 0.005f, 0.06f},
        (float [3]){0.0f, playerRot, 0.0f},
        players[p].position.v
      );
    }

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf());

    t3d_frame_start();
    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color(RGBA32(160, 110, 200, 0xFF));

    t3d_light_set_ambient(colorAmbient);

    t3d_screen_clear_color(RGBA32(160, 110, 200, 0xFF));
    t3d_screen_clear_depth();

    t3d_fog_set_range(12.0f, 85.0f);

    for(int v=0; v<PLAYER_COUNT; ++v)
    {
      T3DViewport *vp = &viewports[v];
      float fov = v == 2 ? T3D_DEG_TO_RAD(50.0f) : T3D_DEG_TO_RAD(75.0f);

      T3DVec3 camTarget = {{
        players[v].position.v[0] + fm_cosf(players[v].rot),
        players[v].position.v[1] + 9.0f,
        players[v].position.v[2] + fm_sinf(players[v].rot)
      }};
      T3DVec3 camPos = {{
        players[v].position.v[0],
        players[v].position.v[1] + 9.0f,
        players[v].position.v[2]
      }};
      // Like in all other examples, set up the projection (only really need to do it once) and view matrix here
      // after that apply the viewport and draw your scene
      // Since each of the viewport-structs has its own matrices, no conflicts will occur
      t3d_viewport_set_projection(vp, fov, 2.0f, 200.0f);
      t3d_viewport_look_at(vp, &camPos, &camTarget, &(T3DVec3){{0,1,0}});
      t3d_viewport_attach(vp);

      // if you need directional light, re-apply it here after a new viewport has been attached
      //t3d_light_set_directional(0, colorDir, &lightDirVec);
      t3d_matrix_push_pos(1);

      // draw player-models
      for(int p=0; p<PLAYER_COUNT; ++p)
      {
        if(p == v)continue;
        t3d_matrix_set(players[p].mat, true);
        rdpq_set_prim_color(players[p].color);
        t3d_matrix_set(players[p].mat, true);
        rspq_block_run(dplPlayer);
      }

      rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));
      t3d_matrix_set(modelMatFP, true);
      rspq_block_run(dplMap);

      t3d_matrix_pop(1);
    }

    // ======== Draw (2D) ======== //
    rdpq_sync_pipe();
    rdpq_set_scissor(0, 0, sizeX, sizeY);
    rdpq_set_mode_standard();
    rdpq_set_mode_fill(RGBA32(0, 0, 0, 0xFF));

    // draw thick lines between the screens
    rdpq_fill_rectangle(0, sizeY/2-1, sizeX, sizeY/2+1);
    rdpq_fill_rectangle(sizeX/2-1, 0, sizeX/2+1, sizeY/2);

    // minimap
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(128);
    rdpq_sync_load();
    rdpq_sprite_blit(spriteMinimap,
      display_get_width()/2 - 29,
      display_get_height()/2 - 29,
    NULL);

    // draw player icons on minimap
    for(int i=0; i<PLAYER_COUNT; ++i) {
      rdpq_set_mode_fill(players[i].color);
      float px = display_get_width()/2 + (players[i].position.v[0] * -0.22f);
      float py = display_get_height()/2 + (players[i].position.v[2] * -0.22f);
      rdpq_fill_rectangle(px-1, py-1, px+2, py+2);
    }

    // draw icons in 3d view of player pos
    for(int i=0; i<PLAYER_COUNT; ++i) {
      if(selPlayer == i)continue;
      T3DVec3 posView;
      T3DVec3 posCenter;
      t3d_vec3_add(&posCenter, &players[selPlayer].position, &(T3DVec3){{0, 6.0f, 0}});
      t3d_viewport_calc_viewspace_pos(&viewports[i], &posView, &posCenter);
      rdpq_set_mode_fill(RGBA32(0x00, 0x00, 0x00, 0xFF));
      posView.v[0] -= 2;
      posView.v[1] -= 2;

      // check if screen-pos is in front of us, and inside the viewport
      if(posView.v[2] < 1.0f
        && posView.v[0] > viewports[i].offset[0] && posView.v[1] > viewports[i].offset[1]
        && posView.v[0] < viewports[i].offset[0] + viewports[i].size[0] - 3
        && posView.v[1] < viewports[i].offset[1] + viewports[i].size[1] - 3
      ) {
        rdpq_fill_rectangle(posView.v[0], posView.v[1], posView.v[0]+3, posView.v[1]+3);
      }
    }

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}