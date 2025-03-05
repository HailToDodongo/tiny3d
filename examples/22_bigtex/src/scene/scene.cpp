/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "scene.h"
#include "../render/fbCpuDraw.h"
#include "../render/debugDraw.h"
#include "debugMenu.h"

namespace {

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

  const char* const MAP_PATHS[3] = {
    "rom:/model.t3dm",
    "rom:/model2.t3dm",
    "rom:/model3.t3dm",
  };

}

Scene::Scene()
  : mapModel{MAP_PATHS[state.mapModel], textures}
{
  skybox.change(state.currSkybox);
}

void Scene::update(float deltaTime) {
  auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
  if(btn.start)state.menuOpen = !state.menuOpen;

  camera.setCamLerpSpeed(state.slowCam ? 0.5f : 0);
  camera.update(deltaTime);
  skybox.update(camera.getPosition());
  lightTimer += deltaTime;
}

void Scene::draw(const Memory::FrameBuffers &buffers, surface_t *surf)
{
  surface_t *surfDepth = Memory::getZBuffer();
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

  t3d_matrix_pop(1);

  rdpq_sync_tile();
  rdpq_sync_load();
  rdpq_set_color_image(surf);
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

