/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "scene.h"
#include "../render/fbCpuDraw.h"
#include "../render/debugDraw.h"
#include "debugMenu.h"

namespace {
  constexpr float PLAYER_SCALE = 0.025f;

  struct LightData {
    color_t lightAmbient{};
    color_t lightDir{};
  };

  constexpr LightData lightData[4] = {
    {{0x99, 0x99, 0x99, 0x99}, {0xFF, 0x99, 0x99, 0xFF}},
    {{0xFF, 0xFF, 0xFF, 0x99}, {0xFF, 0xFF, 0xFF, 0xFF}},
    {{0x44, 0x44, 0x44, 0x99}, {168, 160, 44, 0xFF}},
    {{0x11, 0x11, 0x11, 0x99}, {255, 255, 255, 0xFF}},
  };

  const char* const MAP_PATHS[4] = {
    "rom:/model.t3dm",
    "rom:/model2.t3dm",
    "rom:/model3.t3dm",
    "rom:/room/room.t3dm",
  };

}

Scene::Scene()
  : lightProbes{state.mapModel == 3 ? "rom:/room/light.lpb" : ""},
  mapModel{MAP_PATHS[state.mapModel], textures}
{
  skybox.change(state.currSkybox);
  modelPlayer = t3d_model_load("rom:/player.t3dm");
  rspq_block_begin();
    t3d_model_draw(modelPlayer);
  modelPlayer->userBlock = rspq_block_end();

  playerMatFP.fillSRT({PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE}, {0,0,0}, playerPos);
}

Scene::~Scene() {
  t3d_model_free(modelPlayer);
}

void Scene::update(float deltaTime) {
  auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
  auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
  if(btn.start)state.menuOpen = !state.menuOpen;

  auto oldCamPos = camera.getPosition();

  camera.setCamLerpSpeed(state.slowCam ? 0.08f : 0);
  camera.update(deltaTime);
  skybox.update(camera.getPosition());
  lightTimer += deltaTime;

  if(held.a) {
    auto camDelta = camera.getPosition() - oldCamPos;
    playerPos += camDelta;
    playerPos.y = 4.0;
    t3d_mat4fp_from_srt_euler(playerMatFP.getNext(), {PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE}, {0,0,0}, playerPos);
  }
}

void Scene::draw(const Memory::FrameBuffers &buffers, surface_t *surf)
{
  surface_t *surfDepth = Memory::getZBuffer(state.frame & 1);
  surface_t *surfDepthLast = Memory::getZBuffer(1 - (state.frame & 1));
  uint32_t frameIdxLast = (state.frameIdx + 2) % 3;

  t3d_frame_start();
  rdpq_mode_antialias(AA_NONE);
  rdpq_mode_dithering(DITHER_NONE_NONE);

  uvTex.use();
  camera.attach();

  rdpq_set_color_image(surfDepth);

  rdpq_mode_push();
    rdpq_set_mode_fill(color_from_packed16(0xFFFE));
    rdpq_fill_rectangle(0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
  rdpq_mode_pop();

  T3DVec3 lightDir{
    fm_sinf(lightTimer*0.8f) * 0.75f, 0.75f,
    fm_cosf(lightTimer*0.8f) * 0.75f
  };
  t3d_vec3_norm(lightDir);

  t3d_light_set_ambient(lightData[state.currSkybox].lightAmbient);
  t3d_light_set_directional(0, lightData[state.currSkybox].lightDir, lightDir);
  t3d_light_set_count(0);

  rdpq_set_color_image(&buffers.uv[state.frameIdx]);
  rdpq_set_z_image(surfDepth);

  t3d_matrix_push_pos(1);

  // First draw, this will write the UVs into the offscreen-buffer.
  // Note that this will draw the *next* frame, so that the CPU task that follows doesn't have to wait for it.
  skybox.draw();
  if(state.drawMap)mapModel.draw();

  rdpq_sync_pipe();
  rdpq_mode_zbuf(false, false);

  // Second draw, if shading is enabled, this will draw the lighting + vertex colors into a different buffer
  if(state.drawShade && state.drawMap) {
    rdpq_set_color_image(&buffers.shade[state.frameIdx]);
    rdpq_mode_push();
      rdpq_set_mode_fill({0});
      rdpq_fill_rectangle(0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
    rdpq_mode_pop();
    mapModel.drawShade();
    rdpq_sync_pipe();
  }

  rdpq_sync_tile();
  rdpq_sync_load();
  rdpq_set_color_image(surf);
  rdpq_set_z_image(surfDepthLast);
  rdpq_set_mode_standard();

  rdpq_set_lookup_address(1, buffers.shade[frameIdxLast].buffer);
  rdpq_set_lookup_address(2, surf->buffer);

  rspq_flush();

  uint64_t ticks = get_ticks();
  auto *texIn = (uint64_t*)CachedAddr(buffers.uv[frameIdxLast].buffer);

  uint32_t FB_SIZE_IN = SCREEN_WIDTH * SCREEN_HEIGHT * 4;
  switch(state.drawMode) {
    case State::DRAW_MODE_DEF: FbCPU::applyTextures(texIn, (uint16_t*)surf->buffer, FB_SIZE_IN, fbBlend); break;
    case State::DRAW_MODE_UV:  FbCPU::applyTexturesUV(texIn, (uint16_t*)surf->buffer, FB_SIZE_IN); break;
    case State::DRAW_MODE_MAT: FbCPU::applyTexturesMat(texIn, (uint16_t*)surf->buffer, FB_SIZE_IN); break;
  }
  ticks = get_ticks() - ticks;

  rdpq_sync_pipe();
  rdpq_sync_tile();
  t3d_frame_start();

  rdpq_mode_antialias(AA_NONE);
  rdpq_mode_dithering(DITHER_NONE_NONE);
  rdpq_set_prim_color({0xFF, 0xFF, 0xFF, 0xFF});

  t3d_matrix_pop(1);
  camera.attachLast();
  t3d_matrix_push_pos(1);

  if(state.drawMap) {
    auto ticksP = get_ticks();
    auto probeRes = lightProbes.query(playerPos);
    ticksP = get_ticks() - ticksP;
    if(state.menuOpen && state.frame % 60 == 0) {
      debugf("Probe time: %lldus\n", TICKS_TO_US(ticksP));
    }
    const auto &c = probeRes.colors;

    t3d_light_set_ambient({0,0,0,0});
    t3d_light_set_directional(0, {c[0][0], c[0][1], c[0][2], 0xFF}, {-1,0,0});
    t3d_light_set_directional(1, {c[1][0], c[1][1], c[1][2], 0xFF}, {1,0,0});
    t3d_light_set_directional(2, {c[2][0], c[2][1], c[2][2], 0xFF}, {0,-1,0});
    t3d_light_set_directional(3, {c[3][0], c[3][1], c[3][2], 0xFF}, {0,1,0});
    t3d_light_set_directional(4, {c[4][0], c[4][1], c[4][2], 0xFF}, {0,0,-1});
    t3d_light_set_directional(5, {c[5][0], c[5][1], c[5][2], 0xFF}, {0,0,1});
    t3d_light_set_count(6);

    t3d_matrix_set(playerMatFP.get(), true);
    rspq_block_run(modelPlayer->userBlock);

    t3d_light_set_ambient({0xFF, 0xFF, 0xFF, 0xFF});
    t3d_light_set_count(0);
    mapModel.drawTrans();
  }

  t3d_matrix_pop(1);

  Debug::draw(static_cast<uint16_t *>(surf->buffer));
  Debug::printStart();
  if(state.menuOpen) {
    DebugMenu::draw(skybox);
  }

  switch(state.drawMode)
  {
    case State::DRAW_MODE_DEF: Debug::printf(SCREEN_WIDTH - 46, 18, state.limitFPS ? "%.0f#" : "%.1f", state.fps); break;
    case State::DRAW_MODE_UV : Debug::print(SCREEN_WIDTH - 40, 18, "UV"); break;
    case State::DRAW_MODE_MAT: Debug::print(SCREEN_WIDTH - 80, 18, "Material"); break;
  }

  if((state.frame % 60 == 0) && state.menuOpen) {
    float rec = (TICKS_TO_US(ticks) / 1000.0f);
    if(rec < 0.001f)rec = 0.001f;
    float maxFPS = 1000.0f / rec;

    debugf("Time: %lld | FPS: %.2f (max: %.2f)\n", TICKS_TO_US(ticks), state.fps, maxFPS);
  }
}

