/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneParticle.h"
#include "../../actors/magicSphere.h"
#include "../../actors/pointGlobe.h"
#include "../../main.h"

namespace {
  uint8_t colorAmbient[4] = {0x19, 0x19, 0x19, 0x00};
  int8_t orgPosX[256]{};
  int8_t orgPosZ[256]{};
  int8_t displace[255]{};
}

SceneParticle::SceneParticle()
{
  camera.fov = T3D_DEG_TO_RAD(70.0f);
  camera.near = 2.5f;
  camera.far = 100.0f;

  camera.target = {0,0,0};
  flyCam.camPos = {-70.0, 21.0, 0.0};
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

  float modelScale = 0.15f;
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
  while(t3d_model_iter_next(&it)) {
    rspq_block_begin();
    t3d_model_draw_object(it.object, NULL);
    it.object->userBlock = rspq_block_end();
  }

  constexpr fm_vec3_t pos{0, 40, 0};
  actors.push_back(new Actor::MagicSphere(pos, {.scale = 1.2f, .color = {0x33,0x11,0xFF,0xFF}}));
  actors.push_back(new Actor::MagicSphere(pos, {.scale = 1.25f, .color = {0x33,0x33,0xFF,0xFF}}));
  actors.push_back(new Actor::MagicSphere(pos, {.scale = 1.3f, .color = {0x33,0x55,0xFF,0xFF}}));
  actors.push_back(new Actor::MagicSphere(pos, {.scale = 1.35f, .color = {0x33,0x77,0xFF,0xFF}}));

  actors.push_back(new Actor::PointGlobe(pos, {.scale = 1.0f }));
}

SceneParticle::~SceneParticle()
{
  t3d_model_free(mapModel);
  free_uncached(mapMatFP);
}

void SceneParticle::updateScene(float deltaTime)
{
  flyCam.update(deltaTime);
  lightAngle += deltaTime * 1.5f;

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

void SceneParticle::draw3D(float deltaTime)
{
  t3d_screen_clear_depth();
  rdpq_set_env_color({0x1F, 0x1F, 0x1F, 0x1F});

  t3d_light_set_ambient({0xFF, 0xFF, 0xFF, 0xFF});
  t3d_light_set_count(0);

  t3d_matrix_push(mapMatFP);

  float texScroll = 0.75f * deltaTime;

  auto it = t3d_model_iter_create(mapModel, T3D_CHUNK_TYPE_OBJECT);
  auto modelState = t3d_model_state_create();
  while(t3d_model_iter_next(&it)) {

    auto &texB = it.object->material->textureB;
    texB.s.low += texScroll;
    texB.s.height += texScroll;
    texB.t.low += texScroll;
    texB.t.height += texScroll;

    t3d_model_draw_material(it.object->material, &modelState);
    rspq_block_run(it.object->userBlock);
  }

  for(auto actor : actors)actor->draw3D(deltaTime);

  t3d_matrix_pop(1);

  // particles
  rdpq_sync_pipe();

  rdpq_mode_begin();
    rdpq_mode_zbuf(true, true);
    rdpq_mode_zoverride(true, 0, 0);
    rdpq_mode_alphacompare(20);
    rdpq_mode_persp(false);
    rdpq_mode_filter(FILTER_BILINEAR);
    rdpq_mode_combiner(RDPQ_COMBINER1((0,0,0,PRIM), (0,0,0,1)));
  rdpq_mode_end();

  tpx_state_from_t3d();
  tpx_state_set_scale(0.4f, 1.0f);

  particles.count = particles.countMax;
  //particles.draw();
}