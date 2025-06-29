/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneEnv.h"
#include "../main.h"
#include "../actors/envSphere.h"
#include "../debugMenu.h"
#include <array>

namespace {
  struct Light {
    color_t color{};
    fm_vec3_t dir{};
  };
  struct LightSetup {
    Light lightA{};
    Light lightB{};
  };

  constexpr fm_vec3_t normalize(const fm_vec3_t &v) {
    float len = sqrtf(v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
    return {v.v[0] / len, v.v[1] / len, v.v[2] / len};
  }

  uint8_t colorAmbient[4] = {0, 0, 0, 0};

  LightSetup sceneLights[] = {
    {
      {{0x99, 0xEE, 0xFF, 0}, {0, 1, 0}},
      {{0x2A, 0x29, 0x2F, 0}, {0, -1, 0}},
    }, {
      {{0xA0, 0xDF, 0xBE, 0}, {0, 1, 0}},
      {{0x20, 0x20, 0x20, 0}, {0, -1, 0}},
    }, {
      {{0x3A, 0x10, 0x10, 0}, normalize({0.2f, 1.0f, 0.0f})},
      {{0x80, 0x67, 0x45, 0}, normalize({1, 0.2f, 0.5f})},
    }
  };

  char PATH_BG[]{"rom:/env/bgHamburg00.rgba32.sprite\0"};
  bool paramsChanged = false;
}

SceneEnv::SceneEnv()
{
  state.activeScene = this;
  bgIndex = 2;

  camera.fov = T3D_DEG_TO_RAD(85.0f);
  camera.near = 1.0f;
  camera.far = 100.0f;
  camera.pos = {0.0, 0.0, 20.0};
  camera.target = {0,0,0};

  actors.push_back(new Actor::EnvSphere({}, {.type = 0}));
  actors.push_back(new Actor::EnvSphere({}, {.type = 1}));
  actors.push_back(new Actor::EnvSphere({}, {.type = 2}));

  for(auto &a : actors)a->flags |= Actor::Base::FLAG_DISABLED;

  refreshScene();

  DebugMenu::addEntry({"Scene", DebugMenu::EntryType::INT, &bgIndex, 0, 2}, &paramsChanged);
  DebugMenu::addEntry({"Blur ", DebugMenu::EntryType::BOOL, &showBlurred}, &paramsChanged);
  DebugMenu::addEntry({"Model", DebugMenu::EntryType::BOOL, &showModel});
}

SceneEnv::~SceneEnv()
{
  t3d_model_free(mapModel);
  free_uncached(mapMatFP);

  if(spriteBG)sprite_free(spriteBG);
  if(dplBG)rspq_block_free(dplBG);
}

void SceneEnv::refreshScene()
{
  if(spriteBG) {
    rspq_wait();
    sprite_free(spriteBG);
    rspq_block_free(dplBG);
  }

  PATH_BG[18] = showBlurred ? '1' : '0';
  PATH_BG[19] = '0' + bgIndex;
  spriteBG = sprite_load(PATH_BG);

  rspq_block_begin();
    rdpq_mode_push();
      rdpq_set_mode_standard();
      rdpq_sprite_blit(spriteBG, 0, 0, nullptr);
    rdpq_mode_pop();
  dplBG = rspq_block_end();
}

void SceneEnv::updateScene(float deltaTime)
{
  if(paramsChanged) {
    paramsChanged = false;
    refreshScene();
  }

  for(uint32_t i=0; i<actors.size(); ++i) {
    if(showModel && i == bgIndex) {
      actors[i]->flags &= ~Actor::Base::FLAG_DISABLED;
    } else {
      actors[i]->flags |= Actor::Base::FLAG_DISABLED;
    }
  }

  camera.update(deltaTime);
}

void SceneEnv::draw3D(float deltaTime)
{
  t3d_screen_clear_depth();

  if(dplBG)rspq_block_run(dplBG);

  t3d_light_set_ambient(colorAmbient);

  t3d_light_set_directional(0, {0, 0, 0, 0xFF}, {0, 0, 1});
  t3d_light_set_directional(1, sceneLights[bgIndex].lightA.color, sceneLights[bgIndex].lightA.dir);
  t3d_light_set_directional(2, sceneLights[bgIndex].lightB.color, sceneLights[bgIndex].lightB.dir);
  t3d_light_set_count(0);
}

void SceneEnv::drawPostHDR(float deltaTime)
{

}