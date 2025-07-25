/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "magicSphere.h"
#include "../main.h"

namespace {
  constexpr float BASE_SCALE = 0.32f;
}

// Shared resources:
namespace {
  T3DModel *model{};
  uint32_t refCount{0};
}

namespace Actor
{
  MagicSphere::MagicSphere(const fm_vec3_t &_pos, const Args &_args)
  {
    if(refCount++ == 0) {
      model = t3d_model_load("rom:/magicRing.t3dm");
      rspq_block_begin();
        t3d_model_draw(model);
      model->userBlock = rspq_block_end();
    }

    pos = _pos;
    args = _args;
    timer = (float)(rand() % 256) / 32.0f;

    args.scale *= BASE_SCALE;
  }

  MagicSphere::~MagicSphere()
  {
    if(--refCount == 0) {
      t3d_model_free(model);
      model = nullptr;
    }
  }

  void MagicSphere::update(float deltaTime)
  {
    timer += deltaTime * 0.06f;
    t3d_mat4fp_from_srt_euler(matFP.getNext(),
      {args.scale, args.scale, args.scale},
      {timer, timer*1.2f, timer*1.5f},
      pos
    );
  }

  void MagicSphere::draw3D(float deltaTime)
  {
    if(!checkFrustumSphere(pos, args.scale * 70.0f))return;

    t3d_matrix_set(matFP.get(), true);

    rdpq_set_prim_color(args.color);
    rspq_block_run(model->userBlock);
  }
}