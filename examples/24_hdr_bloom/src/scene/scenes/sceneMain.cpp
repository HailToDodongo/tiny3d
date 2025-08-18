/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneMain.h"
#include "../../main.h"
#include "../../render/debugDraw.h"
#include "../../actors/pointGlobe.h"
#include <string_view>

namespace {
  constexpr uint8_t colorAmbient[4] = {0x2A, 0x2A, 0x2A, 0x00};
  constexpr float modelScale = 0.15f;

  struct ObjectLayer
  {
    T3DObject *objects[32]{};
    uint32_t objCount{0};
  };

  ObjectLayer objLayers[6]{};
  uint32_t triCount{0};
}

SceneMain::SceneMain()
{
  state.ppConf.hdrFactor = 1.7f;
  state.ppConf.blurBrightness = 1.05f;

  camera.fov = T3D_DEG_TO_RAD(80.0f);
  camera.near = 5.0f;
  camera.far = 295.0f;

  //camera.pos = {0.0, -40.0, -400.0};
  camera.pos = {0, 0, 10};
  flyCam.camPos = camera.pos;
  camera.target = {0,0,0};
  flyCam.camRotX = -2.0f;

  mapModel = t3d_model_load("rom://scene.t3dm");
  mapMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

  t3d_mat4fp_from_srt_euler(mapMatFP,
    {modelScale, modelScale, modelScale},
    {0,0,0},
    {0,0,0}
  );

  for(auto &l : objLayers)l = {};

  auto it = t3d_model_iter_create(mapModel, T3D_CHUNK_TYPE_OBJECT);
  while(t3d_model_iter_next(&it)) {
    rspq_block_begin();
    auto mat = it.object->material;

    mat->otherModeMask |= SOM_Z_WRITE | SOM_Z_COMPARE;

    // Some workaround for missing imported settings from fast64
    uint8_t layerIdx = it.object->name[0] - '0';

    it.object->userValue0 = 0;
    it.object->userValue0 |= (it.object->name[2] == 'R') ? SOM_Z_COMPARE : 0;
    it.object->userValue0 |= (it.object->name[3] == 'W') ? SOM_Z_WRITE : 0;

    std::string_view name{it.object->name};
    it.object->userValue1 = name.ends_with("Bottom");

    t3d_model_draw_object(it.object, NULL);
    it.object->userBlock = rspq_block_end();

    auto &layer = objLayers[layerIdx];
    layer.objects[layer.objCount++] = it.object;
  }

  actors.push_back(new Actor::PointGlobe({0, 20, -560}, {.scale = 0.8f}));
}

SceneMain::~SceneMain()
{
  t3d_model_free(mapModel);
  free_uncached(mapMatFP);
}

void SceneMain::updateScene(float deltaTime)
{
  flyCam.update(deltaTime);

  auto frustum = camera.getFrustum();
  t3d_frustum_scale(&frustum, modelScale);

  const T3DBvh *bvh = t3d_model_bvh_get(mapModel); // BVHs are optional, use '--bvh' in the gltf importer (see Makefile)
  assert(bvh != nullptr);
  t3d_model_bvh_query_frustum(bvh, &frustum);
}

void SceneMain::draw3D(float deltaTime)
{
  t3d_screen_clear_color({0,0,0,0xFF});
  t3d_screen_clear_depth();
  rdpq_set_env_color({0xFF, 0xAA, 0xEE, 0xAA});

  t3d_light_set_ambient(colorAmbient);

  t3d_matrix_push(mapMatFP);

  t3d_light_set_count(0);
  int lastLightCount = 0;

  T3DModelState modelState = t3d_model_state_create();

  triCount = 0;
  int layerIdx = -1;
  for(auto &layer : objLayers)
  {
    ++layerIdx;

    // outer sides of the long hallway
    if(layerIdx == 2 && camera.pos.z < -200.0f) {
      continue;
    }
    // black plane to hide long hallway when outside
    if(layerIdx == 4 && camera.pos.z > -260.0f) {
      continue;
    }

    for(uint32_t i=0; i<layer.objCount; ++i)
    {
      auto &obj = *layer.objects[i];
      if(!obj.isVisible)continue;
      obj.isVisible = false;

      // Note: we do this each time here since materials are shared, and this may differ per object
      obj.material->otherModeValue &= ~(SOM_Z_COMPARE | SOM_Z_WRITE);
      obj.material->otherModeValue |= (uint64_t)obj.userValue0;

      //debugf("[%s:%s]: depth: %02X | light: %d\n", obj.name, obj.material->name, obj.userValue0, obj.userValue1);

      t3d_model_draw_material(obj.material, &modelState);

      if(lastLightCount != obj.userValue1) {
        lastLightCount = obj.userValue1;
        t3d_light_set_count(lastLightCount);
      }
      rspq_block_run(obj.userBlock);
      triCount += obj.triCount;
    }
  }

  t3d_matrix_pop(1);
}

void SceneMain::draw2D(float deltaTime)
{
  //Debug::printf(100, 200, "%.2f %.2f %.2f", camera.pos.x, camera.pos.y, camera.pos.z);
  //Debug::printf(100, 210, "Tris: %d\n", triCount);
}