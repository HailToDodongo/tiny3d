/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneMagic.h"
#include <string_view>
#include "../../actors/magicSphere.h"
#include "../../actors/magicSpell.h"
#include "../../main.h"

namespace {
  constexpr float modelScale = 0.15f;

  constexpr fm_vec3_t blToWorld(const fm_vec3_t& posBlender) {
    return {
      posBlender.x * 64 * modelScale,
      posBlender.z * 64 * modelScale,
      -posBlender.y * 64 * modelScale,
    };
  }
}

SceneMagic::SceneMagic()
{
  camera.fov = T3D_DEG_TO_RAD(75.0f);
  camera.near = 3.0f;
  camera.far = 200.0f;

  camera.target = {0,0,0};
  flyCam.camPos = {-50.0, 0.0, 0.0};
  flyCam.camRotX = 0.0f;

  mapModel = t3d_model_load("rom://sceneMagic.t3dm");
  mapMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

  t3d_mat4fp_from_srt_euler(mapMatFP,
    (float[3]){modelScale, modelScale, modelScale},
    (float[3]){0,0,0},
    (float[3]){0,0,0}
  );

  auto it = t3d_model_iter_create(mapModel, T3D_CHUNK_TYPE_OBJECT);
  auto matState = t3d_model_state_create();
  rspq_block_begin();
  while(t3d_model_iter_next(&it)) {
    if(std::string_view{it.object->name} == "Sky") {
      objSky = it.object;
      continue;
    }
    t3d_model_draw_material(it.object->material, &matState);
    t3d_model_draw_object(it.object, NULL);
  }
  t3d_state_set_vertex_fx(T3D_VERTEX_FX_NONE, 0, 0);
  mapModel->userBlock = rspq_block_end();

  assertf(objSky, "Sky object not found in model!");
  rspq_block_begin();
    t3d_model_draw_object(objSky, nullptr);
  objSky->userBlock = rspq_block_end();

  constexpr fm_vec3_t posSp{0, 29, 0};
  actors.push_back(new Actor::MagicSphere(posSp, {.scale = 1.20f, .color = {0x67,0x00,0x80,0xFF}}));
  actors.push_back(new Actor::MagicSphere(posSp, {.scale = 1.25f, .color = {0x67,0x20,0x90,0xFF}}));
  actors.push_back(new Actor::MagicSphere(posSp, {.scale = 1.30f, .color = {0x67,0x40,0xA0,0xFF}}));
  actors.push_back(new Actor::MagicSphere(posSp, {.scale = 1.35f, .color = {0x67,0x50,0xB0,0xFF}}));

  actors.push_back(new Actor::MagicSpell(blToWorld({-4.7f, 5.7f, 2.4f + 0.2f}), {.scale = 1.2f, .color = {0x67,0x50,0xB0,0xFF}}));
  actors.push_back(new Actor::MagicSpell(blToWorld({ 7.2f, 0.9f, 0.8f + 0.2f}), {.scale = 1.3f, .color = {0x67,0x50,0xB0,0xFF}}));
  actors.push_back(new Actor::MagicSpell(blToWorld({ 1.1f,-7.9f,-2.3f + 0.2f}), {.scale = 1.0f, .color = {0x67,0x50,0xB0,0xFF}}));
}

SceneMagic::~SceneMagic()
{
  t3d_model_free(mapModel);
  free_uncached(mapMatFP);
}

void SceneMagic::updateScene(float deltaTime)
{
  flyCam.update(deltaTime);
  lightAngle += deltaTime * 1.5f;

  t3d_mat4fp_from_srt_euler(matFPSky.getNext(),
    {modelScale, modelScale, modelScale},
    {0,0,0},
    camera.pos
  );
}

void SceneMagic::draw3D(float deltaTime)
{
  t3d_screen_clear_depth();
  t3d_screen_clear_color({0,0,0, 0xFF});
  rdpq_set_env_color({0xFF, 0xFF, 0xFF, 0xFF});

  t3d_light_set_ambient({0x4F, 0x4F, 0x4F, 0xFF});
  t3d_light_set_count(0);

  t3d_matrix_push(matFPSky.get());

  // Skybox, rotate texture
  auto &texA = objSky->material->textureA;
  texA.s.low += deltaTime * 4.0f;
  if(texA.s.low > objSky->material->textureA.texWidth*2) {
    texA.s.low -= objSky->material->textureA.texWidth*2;
  }
  t3d_model_draw_material(objSky->material, nullptr);
  rdpq_mode_zbuf(false, false);
  rspq_block_run(objSky->userBlock);

  rdpq_sync_pipe();
  rdpq_mode_zbuf(true, true);

  t3d_matrix_set(mapMatFP, true);
  rspq_block_run(mapModel->userBlock);

  t3d_matrix_pop(1);
}