#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

typedef struct {
  T3DVec3 position;
  float rot;
  T3DMat4FP* mat;
  color_t color;
} Player;

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  surface_t depthBuffer = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

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

  T3DViewport viewports[3] = {t3d_viewport_create(), t3d_viewport_create(), t3d_viewport_create()};
  t3d_viewport_set_area(&viewports[0], 0,       0,       sizeX/2, sizeY/2);
  t3d_viewport_set_area(&viewports[1], sizeX/2, 0,       sizeX/2, sizeY/2);
  t3d_viewport_set_area(&viewports[2], 0,       sizeY/2, sizeX,   sizeY/2-2);

  Player players[3] = {
    {{{0,  6,  0}}, 0, malloc_uncached(sizeof(T3DMat4FP)), RGBA32(220, 100, 100, 0xFF)},
    {{{0,  6, 40}}, 0, malloc_uncached(sizeof(T3DMat4FP)), RGBA32(100, 200, 100, 0xFF)},
    {{{20, 6, 20}}, 0, malloc_uncached(sizeof(T3DMat4FP)), RGBA32(200, 200, 100, 0xFF)},
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
    }

    // ======== Draw ======== //
    rdpq_attach(display_get(), &depthBuffer);

    t3d_frame_start();
    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color(RGBA32(110, 110, 200, 0xFF));

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(1);

    t3d_screen_clear_color(RGBA32(110, 110, 200, 0xFF));
    t3d_screen_clear_depth();

    t3d_fog_set_range(5.0f, 25.0f);

    for(int v=0; v<3; ++v)
    {
      T3DViewport *vp = &viewports[v];
      float fov = v == 2 ? T3D_DEG_TO_RAD(50.0f) : T3D_DEG_TO_RAD(75.0f);

      T3DVec3 camTarget = {{fm_cosf(players[v].rot), 0.0f, fm_sinf(players[v].rot)}};
      t3d_vec3_add(&camTarget, &camTarget, &players[v].position);

      t3d_viewport_set_projection(vp, fov, 2.0f, 200.0f);
      t3d_viewport_look_at(vp, &players[v].position, &camTarget);
      //rspq_wait();
      t3d_viewport_apply(vp);

      t3d_light_set_directional(0, colorDir, &lightDirVec);

      for(int p=0; p<3; ++p)
      {
        if(p == v)continue;
        t3d_matrix_set_mul(players[p].mat, 1, 0);
        rdpq_set_prim_color(players[p].color);
        t3d_matrix_set_mul(players[p].mat, 1, 0);
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

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

