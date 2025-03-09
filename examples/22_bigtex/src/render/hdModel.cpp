/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <vector>
#include <algorithm>
#include "hdModel.h"
#include "../main.h"

namespace {
  constexpr float modelScale = 0.15f;
}

HDModel::HDModel(const std::string &t3dmPath, Textures& textures) {
  model = t3d_model_load(t3dmPath.c_str());

  std::vector<T3DObject*> objects{};
  auto it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);
  while(t3d_model_iter_next(&it)) {
    objects.push_back(it.object);
  }
  // sort by name
  std::sort(objects.begin(), objects.end(), [](const T3DObject *a, const T3DObject *b) {
    if(a->material->name[0] == b->material->name[0]) {
      return a->material->name[1] < b->material->name[1];
    }
    return a->material->name[0] < b->material->name[0];
  });

  uint8_t baseAddrMat = (TEX_BASE_ADDR >> 16) & 0xFF;
  for(auto &obj : objects)
  {
    auto *mat = obj->material;
    if(mat->textureA.texReference == 0xFF)continue;
    //debugf("Mat: %s %d\n", mat->name, mat->textureA.texWidth);
    if(mat->textureA.texWidth != 256)continue;

    uint8_t matIdx = 0;
    if (mat->textureA.texReference) {
      matIdx = textures.reserveTexture();
      //debugf("Tex[%d]: <placeholder> (%s)\n", matIdx, mat->name);
    } else {
      std::string bc1Path{mat->textureA.texPath};
      bc1Path = bc1Path.substr(0, bc1Path.find_last_of('.'));
      matIdx = textures.addTexture(bc1Path);
      //debugf("Tex[%d]: %s (%s)\n", matIdx, mat->textureA.texPath, mat->name);
    }

    mat->otherModeMask |= SOM_Z_COMPARE | SOM_Z_WRITE;

    if(mat->name[0] == '_') {
      mat->otherModeValue &= ~(SOM_Z_COMPARE | SOM_Z_WRITE);
    } else if(mat->name[0] == '#') {
      mat->otherModeValue &= ~(SOM_Z_COMPARE);
      mat->otherModeValue |= SOM_Z_WRITE;
    } else {
      mat->otherModeValue |= SOM_Z_COMPARE | SOM_Z_WRITE;
    }

    mat->otherModeMask |= SOM_SAMPLE_MASK;
    mat->otherModeValue |= SOM_SAMPLE_POINT;

    // Override material for UV texture gradients
    mat->renderFlags &= ~T3D_FLAG_SHADED;
    mat->textureA.texPath = nullptr;
    mat->textureB.texPath = nullptr;
    mat->textureA.texReference = 0xFF;
    mat->textureB.texReference = 0xFF;

    mat->primColor = {(uint8_t)(baseAddrMat + matIdx),0,0,0xFF};
    mat->colorCombiner = RDPQ_COMBINER2(
      (1, 0, TEX0, TEX1),     (0,0,0,1),
      (1, 0, PRIM, COMBINED), (0,0,0,1)
    );

    ++matIdx;
  }

  matFP.fillSRT(
    {modelScale, modelScale, modelScale},
    {}, {}
  );

  rspq_block_begin();
    auto t3dState = t3d_model_state_create();
    for(auto obj : objects) {
      if(obj->material->textureA.texWidth == 256) {
        t3d_model_draw_material(obj->material, &t3dState);
        t3d_model_draw_object(obj, nullptr);
      }
    }
    t3d_state_set_vertex_fx(T3D_VERTEX_FX_NONE, 0,0);
  dplDraw = rspq_block_end();

  rspq_block_begin();
  t3dState = t3d_model_state_create();
  for(auto obj : objects) {
    if(obj->material->textureA.texWidth != 256) {
      t3d_model_draw_material(obj->material, &t3dState);
      t3d_model_draw_object(obj, nullptr);
    }
  }
  t3d_state_set_vertex_fx(T3D_VERTEX_FX_NONE, 0,0);
  dplDrawTrans = rspq_block_end();

  rspq_block_begin();
    rdpq_sync_pipe();
    rdpq_mode_combiner(RDPQ_COMBINER1((1, SHADE, PRIM, 0), (0,0,0,1)));
    rdpq_mode_blender(0);
    rdpq_mode_alphacompare(0);

    rdpq_set_prim_color({0xFF, 0xFF, 0xFF, 0xFF});
    t3d_state_set_drawflags((T3DDrawFlags)(T3D_FLAG_DEPTH | T3D_FLAG_SHADED | T3D_FLAG_CULL_BACK));
    t3d_light_set_count(1);

    int lastNoDepth = -1;
    for(auto obj : objects) {
        int noDepth = obj->material->name[0] == '_' ? 1 : 0;
        if(noDepth != lastNoDepth)
        {
          rdpq_sync_pipe();
          if(noDepth) {
            rdpq_change_other_modes_raw(SOM_ZMODE_MASK  | SOM_Z_COMPARE | SOM_Z_WRITE, 0);
          } else {
            rdpq_change_other_modes_raw(SOM_ZMODE_MASK  | SOM_Z_COMPARE | SOM_Z_WRITE, SOM_ZMODE_DECAL | SOM_Z_COMPARE);
          }

          lastNoDepth = noDepth;
        }

        t3d_model_draw_object(obj, nullptr);
    }
  dplDrawShade = rspq_block_end();
}

HDModel::~HDModel() {
  rspq_block_free(dplDraw);
  rspq_block_free(dplDrawShade);
  t3d_model_free(model);
}

void HDModel::draw() {
  t3d_matrix_set(matFP.get(), true);
  rspq_block_run(dplDraw);
}

void HDModel::drawShade() {
  t3d_matrix_set(matFP.get(), true);
  rspq_block_run(dplDrawShade);
}

void HDModel::setPos(const T3DVec3 &pos) {
  t3d_mat4fp_set_pos(matFP.getNext(), pos);
}

void HDModel::drawTrans() {
  t3d_matrix_set(matFP.get(), true);
  rspq_block_run(dplDrawTrans);
}
