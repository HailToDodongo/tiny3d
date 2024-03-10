#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

/**
 * Example showcasing how to change the viewport settings to implement a split-screen.
 */

typedef struct {
  int32_t start[2];
  int32_t end[2];
  float fov;
  color_t clearColor;
  T3DMat4FP *projMat;
  T3DMat4FP *viewMat;
} Viewport;

typedef struct {
  T3DVec3 position;
  float rot;
  T3DMat4FP* mat;
} Player;

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  rdpq_init();
  //rdpq_debug_start();

  joypad_init();
  t3d_init();

  T3DMat4 tmpMatrix;

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
  t3d_mat4_identity(&tmpMatrix);
  t3d_mat4_scale(&tmpMatrix, 0.1f, 0.1f, 0.1f);
  t3d_mat4_to_fixed(modelMatFP, &tmpMatrix);

  int sizeX = display_get_width();
  int sizeY = display_get_height();

  // Define a few viewports here, this sets the start (upper-left) and end (lower-right) coordinates.
  Viewport viewports[3] = {
    {{0,       0      }, {sizeX/2,   sizeY/2}, T3D_DEG_TO_RAD(75.0f), RGBA32(220, 100, 100, 0xFF), malloc_uncached(sizeof(T3DMat4FP)), malloc_uncached(sizeof(T3DMat4FP))},
    {{sizeX/2, 0      }, {sizeX,     sizeY/2}, T3D_DEG_TO_RAD(75.0f), RGBA32(100, 200, 100, 0xFF), malloc_uncached(sizeof(T3DMat4FP)), malloc_uncached(sizeof(T3DMat4FP))},
    {{0,       sizeY/2}, {sizeX,     sizeY-2}, T3D_DEG_TO_RAD(50.0f), RGBA32(200, 200, 100, 0xFF), malloc_uncached(sizeof(T3DMat4FP)), malloc_uncached(sizeof(T3DMat4FP))},
  };

  Player players[3] = {
    {{{0,  6,  0}}, 0, malloc_uncached(sizeof(T3DMat4FP))},
    {{{0,  6, 40}}, 0, malloc_uncached(sizeof(T3DMat4FP))},
    {{{20, 6, 20}}, 0, malloc_uncached(sizeof(T3DMat4FP))},
  };

  uint8_t colorAmbient[4] = {180, 180, 240, 0xFF};
  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  T3DModel *model = t3d_model_load("rom:/model.t3dm");
  rspq_block_begin();
  t3d_model_draw(model);
  rspq_block_t *dplDraw = rspq_block_end();

  T3DModel *modelPlayer = t3d_model_load("rom:/cube.t3dm");
  rspq_block_begin();
  t3d_model_draw(modelPlayer);
  rspq_block_t *dplPlayer = rspq_block_end();

  t3d_screen_set_size(display_get_width(), display_get_height(), 1, true);

  int currentViewport = 0;
  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
    if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

    if(btn.l)--currentViewport;
    if(btn.r)++currentViewport;
    if(currentViewport < 0)currentViewport = 2;
    if(currentViewport > 2)currentViewport = 0;

    players[currentViewport].rot += joypad.stick_x * -0.0004f;
    // move player in direction of the camera
    T3DVec3 moveDir = {{
      fm_cosf(players[currentViewport].rot) * (joypad.stick_y * 0.004f),
      0.0f,
      fm_sinf(players[currentViewport].rot) * (joypad.stick_y * 0.004f)
    }};
    t3d_vec3_add(&players[currentViewport].position, &players[currentViewport].position, &moveDir);


    for(int p=0; p<3; ++p) {
      t3d_mat4_identity(&tmpMatrix);
      t3d_mat4_translate(&tmpMatrix, players[p].position.v[0], players[p].position.v[1], players[p].position.v[2]);
      t3d_mat4_scale(&tmpMatrix, 0.04f, 0.04f, 0.04f);
      t3d_mat4_to_fixed(players[p].mat, &tmpMatrix);
      t3d_matrix_set_mul(players[p].mat, 1, 0);
    }

    // ======== Draw ======== //
    t3d_frame_start();
    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color(RGBA32(110, 110, 200, 0xFF));

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    rdpq_sync_pipe();
    t3d_screen_clear_color(RGBA32(110, 110, 200, 0xFF));
    t3d_screen_clear_depth();

    t3d_fog_set_range(5.0f, 25.0f);

    for(int v=0; v<3; ++v)
    {
      Viewport* vp = &viewports[v];
      rspq_wait();

      t3d_screen_set_rect(
        vp->start[0], vp->start[1], vp->end[0], vp->end[1],
        2, false
      );

      float aspectRatio = (float)(vp->end[0] - vp->start[0]) /
                          (float)(vp->end[1] - vp->start[1]);

      t3d_mat4_perspective(&tmpMatrix, vp->fov, aspectRatio, 2.0f, 100.0f);
      t3d_mat4_to_fixed(vp->projMat, &tmpMatrix);
      t3d_matrix_set_proj(vp->projMat);

      T3DVec3 camTarget = {{
        fm_cosf(players[v].rot),
        0.0f,
        fm_sinf(players[v].rot)
      }};
      t3d_vec3_add(&camTarget, &camTarget, &players[v].position);

      t3d_mat4_look_at(&tmpMatrix, &players[v].position, &camTarget);
      t3d_mat4_to_fixed(vp->viewMat, &tmpMatrix);
      t3d_mat_set(vp->viewMat, 0);

      for(int p=0; p<3; ++p)
      {
        if(p == v)continue;
        t3d_matrix_set_mul(players[p].mat, 1, 0);
        rdpq_set_prim_color(viewports[p].clearColor);
        rspq_block_run(dplPlayer);
      }

      rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));
      t3d_matrix_set_mul(modelMatFP, 1, 0);
      rspq_block_run(dplDraw);
    }

    rdpq_sync_pipe();
    rdpq_set_scissor(0, 0, sizeX, sizeY);
    rdpq_set_mode_standard();
    rdpq_set_mode_fill(RGBA32(0, 0, 0, 0xFF));
    // draw thick lines between the screens

    rdpq_fill_rectangle(0, sizeY/2-1, sizeX, sizeY/2+1);
    rdpq_fill_rectangle(sizeX/2-1, 0, sizeX/2+1, sizeY/2);

    // draw border around active viewport
    Viewport *vp = &viewports[currentViewport];
    rdpq_set_mode_fill(vp->clearColor);
    rdpq_fill_rectangle(vp->start[0]+1, vp->start[1], vp->end[0]-1, vp->start[1]+2);
    rdpq_fill_rectangle(vp->start[0]+1, vp->end[1]-2, vp->end[0]-1, vp->end[1]);
    rdpq_fill_rectangle(vp->start[0], vp->start[1]+1, vp->start[0]+2, vp->end[1]-1);
    rdpq_fill_rectangle(vp->end[0]-2, vp->start[1]+1, vp->end[0], vp->end[1]-1);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

