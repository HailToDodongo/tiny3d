/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "envSphere.h"
#include "../main.h"
#include "../scenes/scene.h"

namespace {
  constexpr float BASE_SCALE = 0.04f;
}

// Shared resources:
namespace {
  T3DModel *models[3]{};
  uint32_t refCount{0};

  fm_vec3_t realPos{};
}

namespace Actor
{
  EnvSphere::EnvSphere(const fm_vec3_t &_pos, const Args &_args)
  {
    if(refCount++ == 0) {
      models[0] = t3d_model_load("rom:/envPot.t3dm");
      models[1] = t3d_model_load("rom:/envSphere.t3dm");
      models[2] = t3d_model_load("rom:/envTorus.t3dm");
      for(auto m : models) {
        rspq_block_begin();
          t3d_model_draw(m);
        m->userBlock = rspq_block_end();
      }
    }

    pos = _pos;
    realPos = pos;
    args = _args;
    timer = 2.0f;
  }

  EnvSphere::~EnvSphere()
  {
    if(--refCount == 0) {
      for(auto &m : models) {
        t3d_model_free(m);
        m = nullptr;
      }
    }
  }

  void EnvSphere::update(float deltaTime)
  {
    auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
    auto stick = joypad_get_inputs(JOYPAD_PORT_1);

    if(!held.z) {
      timer += deltaTime * 0.35f;
    }

    pos = {stick.stick_x / 8.0f, stick.stick_y / 8.0f, 0};

    t3d_vec3_lerp(&realPos, &realPos, &pos, 0.15f);
    t3d_mat4fp_from_srt_euler(matFP.getNext(),
      {BASE_SCALE, BASE_SCALE, BASE_SCALE},
      {timer, timer*1.2f, timer*0.7f},
      realPos
    );
  }

  void EnvSphere::draw3D(float deltaTime)
  {
    t3d_matrix_set(matFP.get(), true);

    t3d_light_set_count(3);
    rspq_block_run(models[args.type]->userBlock);
    t3d_light_set_count(0);
  }
}