/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "lightBlock.h"
#include "../main.h"

namespace {
  constexpr float BASE_SCALE = 0.06f;
}

// Shared resources:
namespace {
  T3DModel *model{nullptr};
  uint32_t refCount{0};
}

namespace Actor
{
  LightBlock::LightBlock(const fm_vec3_t &_pos, const Args &_args)
  {
    if(refCount++ == 0) {
      model = t3d_model_load("rom:/light.t3dm");
      rspq_block_begin();
        t3d_model_draw(model);
      model->userBlock = rspq_block_end();
    }

    pos = _pos;
    args = _args;

    timer = 0.0f;
    args.color.a = 0;
  }

  LightBlock::~LightBlock()
  {
    if(--refCount == 0) {
      t3d_model_free(model);
      model = nullptr;
    }
  }

  void LightBlock::update(float deltaTime)
  {
    timer += deltaTime * 0.4f;

    float scale = fm_sinf(timer * 3.0f) * 0.5f + 0.5f;
    scale += 1.0f;

    pos.x = fm_sinf(timer * 1.0f) * 25.0f;
    pos.y = 2 + fm_sinf(timer * 6.0f) * 4.0f;
    pos.z = fm_cosf(timer * 1.0f) * 25.0f;

    t3d_mat4fp_from_srt_euler(matFP.getNext(),
      {BASE_SCALE * scale, BASE_SCALE * scale, BASE_SCALE * scale},
      {timer, timer*1.2f, timer*0.7f},
      pos
    );

    t3d_light_set_point(args.index,   args.color,   pos, scale * 0.042f, false);
    t3d_light_set_point(args.index+1, {0,0,0,0xFF}, pos, scale * 0.03f, true);
  }

  void LightBlock::draw3D(float deltaTime)
  {
    rdpq_set_prim_color({args.color.r, args.color.g,args.color.b, 0x10});
    t3d_matrix_set(matFP.get(), true);
    rspq_block_run(model->userBlock);
  }
}