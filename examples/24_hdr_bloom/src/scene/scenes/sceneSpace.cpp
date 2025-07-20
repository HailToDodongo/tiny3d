/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneSpace.h"
#include <string_view>
#include "../../main.h"
#include "../../debugMenu.h"
#include "../../actors/lightBlock.h"

namespace {
  bool useAmbientLight = false;
  fm_vec3_t lightDir{};
}

SceneSpace::SceneSpace()
{

  camera.fov = T3D_DEG_TO_RAD(70.0f);
  camera.near = 1.0f;
  camera.far = 100.0f;
  camera.pos = {-50.0, 50.0, 50.0};
  camera.target = {0,0,0};

  flyCam.camPos = camera.pos;
  flyCam.camRotX = -1.0f;
  flyCam.camRotY = -0.3f;

  timerSky = 0;
  lightDir = {0.2f,1,0.4f};
  fm_vec3_norm(&lightDir, &lightDir);

  model = t3d_model_load("rom:/skyAnim/scene.t3dm");
  DebugMenu::addEntry({"Ambient", DebugMenu::EntryType::BOOL, &useAmbientLight});
  T3DObject *objSkyTop{};

  auto it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);
  auto matState = t3d_model_state_create();
  rspq_block_begin();
  while(t3d_model_iter_next(&it)) {
    if(std::string_view{it.object->name} == "Sky") {
      objSky = it.object;
      continue;
    }
    if(std::string_view{it.object->name} == "SkyTop") {
      objSkyTop = it.object;
      continue;
    }

    it.object->material->fogMode = 0;
    t3d_model_draw_material(it.object->material, &matState);
    t3d_model_draw_object(it.object, NULL);
  }
  t3d_state_set_vertex_fx(T3D_VERTEX_FX_NONE, 0, 0);
  model->userBlock = rspq_block_end();

  assertf(objSky, "Sky object not found in model!");
  assertf(objSkyTop, "SkyTop object not found in model!");

  rspq_block_begin();
    t3d_model_draw_object(objSkyTop, nullptr);
    t3d_model_draw_object(objSky, nullptr);
  objSky->userBlock = rspq_block_end();

  actors.push_back(new Actor::LightBlock({0,0,0}, {{0xFF, 0x77, 0xAA, 0xFF}, 1}));
}

SceneSpace::~SceneSpace()
{
  t3d_model_free(model);
}

void SceneSpace::updateScene(float deltaTime)
{
  timerSky += deltaTime * 2.0f;
  if(timerSky > 32.0f) {
    timerSky -= 32.0f;
  }

  flyCam.update(deltaTime);
  camera.update(deltaTime);

  constexpr float MAP_SCALE = 0.04f;
  t3d_mat4fp_from_srt_euler(matFP.getNext(),
      {MAP_SCALE, MAP_SCALE, MAP_SCALE},
      {0,0,0},
      {0,0,0}
  );

  t3d_mat4fp_from_srt_euler(matFPSky.getNext(),
    {MAP_SCALE, MAP_SCALE, MAP_SCALE},
    {0,0,0},
    camera.pos
  );
}

void SceneSpace::draw3D(float deltaTime)
{
  t3d_screen_clear_depth();

  t3d_light_set_count(0);
  if(useAmbientLight) {
    t3d_light_set_ambient({0xAA, 0xAA, 0xAA, 0x00});
  } else {
    t3d_light_set_ambient({0x20, 0x20, 0x20, 0x00});
  }

  t3d_matrix_push(matFPSky.get());

  // Skybox, move pixel texture
  auto &texB = objSky->material->textureB;
  texB.t.low = fm_floorf(timerSky);
  texB.s.low = texB.t.low;

  t3d_model_draw_material(objSky->material, nullptr);
  rdpq_mode_zbuf(false, false);
  rspq_block_run(objSky->userBlock);

   // Main map mesh
  rdpq_sync_pipe();
  rdpq_mode_zbuf(true, true);
  rdpq_set_prim_color({0xFF, 0xFF, 0xFF, 0xFF});

  if(useAmbientLight) {
    t3d_light_set_ambient({0x20, 0x20, 0x20, 0x00});
    t3d_light_set_directional(0, {0x50, 0x50, 0x50, 0}, lightDir);
  } else {
    t3d_light_set_ambient({0,0,0,0});
    t3d_light_set_directional(0, {0xA, 0xA, 0xA, 0}, lightDir);
  }
  t3d_light_set_count(2);

  t3d_matrix_set(matFP.get(), true);
  rspq_block_run(model->userBlock);

  t3d_matrix_pop(1);
}
