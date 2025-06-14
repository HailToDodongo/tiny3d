/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneMain.h"

namespace {
  uint8_t colorDir[4] = {0x1F, 0x1F, 0x1F, 0};
  T3DVec3 lightDirVec{0.0f, 1.0f, 0.0f};

  uint8_t lightPointColor[4] = {0xFF, 0x77, 0xFF, 0};
  T3DVec3 lightPointPos = {{4.0f, 10.0f, 0.0f}};

  uint8_t lightPointColor2[4] = {0x77, 0x77, 0xFF, 0};
  T3DVec3 lightPointPos2 = {{4.0f, 9.0f, 0.0f}};

  T3DVec3 camPos = {{-35.0, 21.0, 40.0}};
}

SceneMain::SceneMain()
{
  mapModel = t3d_model_load("rom://scene.t3dm");
  mapMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

  float modelScale = 0.15f;
  t3d_mat4fp_from_srt_euler(mapMatFP,
    (float[3]){modelScale, modelScale, modelScale},
    (float[3]){0,0,0},
    (float[3]){0,0,0}
  );

  T3DModelState modelState = t3d_model_state_create();
  auto it = t3d_model_iter_create(mapModel, T3D_CHUNK_TYPE_OBJECT);

  while(t3d_model_iter_next(&it)) {
    rspq_block_begin();
    auto mat = it.object->material;

    bool hasFresnel = strcmp(mat->name, "fres") == 0;
    // store the light-setup ID in one of the user values
    it.object->userValue0 = hasFresnel ? 1 : 0;

    t3d_model_draw_material(it.object->material, &modelState);
    t3d_model_draw_object(it.object, NULL);
    it.object->userBlock = rspq_block_end();
  }
}

SceneMain::~SceneMain()
{
  t3d_model_free(mapModel);
  free_uncached(mapMatFP);
}

void SceneMain::update(float deltaTime)
{
  lightAngle += deltaTime * 1.5f;
  lightPointPos.x = fm_cosf(lightAngle) * 40.0f;
  lightPointPos.z = fm_sinf(lightAngle) * 40.0f;

  lightPointPos2.x = fm_cosf(lightAngle * -1.2f) * 35.0f;
  lightPointPos2.z = fm_sinf(lightAngle * -1.2f) * 35.0f;
}

void SceneMain::draw(float deltaTime)
{
  rdpq_set_env_color({0xFF, 0xAA, 0xEE, 0xFF});

  t3d_matrix_push(mapMatFP);

  uint8_t lastLightID = 0xFF;
  auto it = t3d_model_iter_create(mapModel, T3D_CHUNK_TYPE_OBJECT);
  while(t3d_model_iter_next(&it))
  {
    if(it.object->userValue0 != lastLightID) {
      lastLightID = it.object->userValue0;
      if(lastLightID == 0) {
        t3d_light_set_point(0, lightPointColor, lightPointPos, 0.09f, false);
        t3d_light_set_point(1, lightPointColor2, lightPointPos2, 0.07f, false);
        t3d_light_set_count(2);
      } else {
        uint8_t col0[4]{0x00, 0x00, 0x00, 0xF0};
        uint8_t col1[4]{0x00, 0x00, 0x00, 0x90};
        t3d_light_set_point(0, col0, {{camPos.x, camPos.y+0.1f, camPos.z+0.1f}}, 1.0f, false);
        t3d_light_set_point(1, col1, {{camPos.x, camPos.y+0.1f, camPos.z+0.1f}}, 1.0f, false);
        t3d_light_set_count(2);
      }
    }
    rspq_block_run(it.object->userBlock);
  }
  t3d_matrix_pop(1);
}