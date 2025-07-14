/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneMagic.h"
#include <string_view>
#include "../../actors/magicSphere.h"
#include "../../actors/pointGlobe.h"
#include "../../main.h"

namespace {
  constexpr float modelScale = 0.15f;

  int8_t orgPosX[256]{};
  int8_t orgPosZ[256]{};
  int8_t displace[255]{};
}

SceneMagic::SceneMagic()
{
  camera.fov = T3D_DEG_TO_RAD(75.0f);
  camera.near = 3.0f;
  camera.far = 200.0f;

  camera.target = {0,0,0};
  flyCam.camPos = {-50.0, 0.0, 0.0};
  flyCam.camRotX = 0.0f;

  for(uint32_t i=0; i<particles.countMax; ++i) {
    auto p = tpx_buffer_get_pos(particles.particles, i);
    auto col = tpx_buffer_get_rgba(particles.particles, i);
    *tpx_buffer_get_size(particles.particles, i) = 6 + (rand()%4);

    float randAngle = (rand() % 1024) / 1024.0f * T3D_PI * 2.0f;
    float randX = fm_sinf(randAngle);
    float randZ = fm_cosf(randAngle);
    int randRad = (rand() % 126);
    p[0] = randX * randRad;
    p[1] = (rand() % 255) - 127;
    p[2] = randZ * randRad;

    orgPosX[i] = p[0];
    orgPosZ[i] = p[2];

    col[0] = 0x33 + (randRad & 0b11);
    col[1] = 0xFF - (randRad & 0b111);
    col[2] = 0x55 + (randRad & 0b1111);
    col[3] = 1 + (rand() % 3);
  }

  for(int i=0; i<255; ++i) {
    displace[i] = fm_sinf((i - 127) * 0.1f) * 4.0f;
  }

  mapModel = t3d_model_load("rom://sceneMagic.t3dm");
  mapMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

  t3d_mat4fp_from_srt_euler(mapMatFP,
    (float[3]){modelScale, modelScale, modelScale},
    (float[3]){0,0,0},
    (float[3]){0,0,0}
  );
  t3d_mat4fp_from_srt_euler(particles.mat,
    (float[3]){0.15f, 0.2f, 0.15f},
    (float[3]){0,0,0},
    (float[3]){0,25.0f,0}
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

  // generate particles
  for(uint32_t i=0; i<particles.countMax; ++i) {
    auto p = tpx_buffer_get_pos(particles.particles, i);
    auto col = tpx_buffer_get_rgba(particles.particles, i);
    int8_t wiggleX = displace[(p[1] + 127) & 0xFF];
    int8_t wiggleZ = displace[(p[1] + 200) & 0xFF];
    p[1] += col[3];
    p[0] = orgPosX[i] + wiggleX;
    p[2] = orgPosZ[i] + wiggleZ;
  }
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