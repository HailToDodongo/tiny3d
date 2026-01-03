/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "../scene/scene.h"
#include "magicSpell.h"
#include "../main.h"

namespace {
  constexpr float BASE_SCALE = 0.2f;
  constexpr float ROT_SPEED = 0.1f;
}

// Shared resources:
namespace {
  T3DModel *model{};
  T3DObject *obj{};
  uint32_t refCount{0};
}

namespace Actor
{
  MagicSpell::MagicSpell(const fm_vec3_t &_pos, const Args &_args)
  {
    if(refCount++ == 0) {
      model = t3d_model_load("rom:/magic.t3dm");
      obj = t3d_model_get_object_by_index(model, 0);
      rspq_block_begin();
        t3d_model_draw_object(obj, nullptr);
      obj->userBlock = rspq_block_end();
    }

    pos = _pos;
    args = _args;
    timer = (float)(rand() % 256) / 32.0f;

    args.scale *= BASE_SCALE;

    for(uint32_t i=0; i<particles.countMax; ++i) {
      auto p = tpx_buffer_s8_get_pos(particles.particles, i);
      auto col = tpx_buffer_s8_get_rgba(particles.particles, i);
      *tpx_buffer_s8_get_size(particles.particles, i) = 6 + (rand()%4);

      float randAngle = (rand() % 1024) / 1024.0f * T3D_PI * 2.0f;
      float randX = fm_sinf(randAngle);
      float randZ = fm_cosf(randAngle);
      int randRad = (rand() % 126);
      p[0] = randX * randRad;
      p[1] = (rand() % 255) - 127;
      p[2] = randZ * randRad;

      orgPosX[i] = p[0];
      orgPosZ[i] = p[2];

      col[0] = args.color.r + (randRad & 0b11);
      col[1] = args.color.g - (randRad & 0b111);
      col[2] = args.color.b + (randRad & 0b1111);
      col[3] = 1 + (rand() % 3);
    }

    for(int i=0; i<255; ++i) {
      displace[i] = fm_sinf((i - 127) * 0.1f) * 4.0f;
    }

    t3d_mat4fp_from_srt_euler(particles.mat,
      fm_vec3_t{1.0f, 1.5f, 1.0f} * args.scale,
      {0,0,0},
      pos + (fm_vec3_t{0, 190.0f, 0} * args.scale)
    );
  }

  MagicSpell::~MagicSpell()
  {
    if(--refCount == 0) {
      t3d_model_free(model);
      model = nullptr;
    }
  }

  void MagicSpell::update(float deltaTime)
  {
    timer += deltaTime * ROT_SPEED;
    t3d_mat4fp_from_srt_euler(matFP.getNext(),
      {args.scale, args.scale, args.scale},
      {0,timer,0},
      pos
    );

    for(uint32_t i=0; i<particles.countMax; ++i) {
      auto p = tpx_buffer_s8_get_pos(particles.particles, i);
      auto col = tpx_buffer_s8_get_rgba(particles.particles, i);
      int8_t wiggleX = displace[(p[1] + 127) & 0xFF];
      int8_t wiggleZ = displace[(p[1] + 200) & 0xFF];
      p[1] += col[3];
      p[0] = orgPosX[i] + wiggleX;
      p[2] = orgPosZ[i] + wiggleZ;
    }
    particles.count = particles.countMax;
  }

  void MagicSpell::draw3D(float deltaTime)
  {
    if(!checkFrustumSphere(pos, args.scale * 90.0f))return;

    t3d_matrix_set(matFP.get(), true);
    rdpq_set_prim_color(args.color);

    auto &tex = obj->material->textureB;
    tex.s.low = timer * 50.0f;
    tex.t.low = timer * 40.0f;
    tex.s.low = fmodf(tex.s.low, tex.texWidth * 2.0f);
    tex.t.low = fmodf(tex.t.low, tex.texHeight * 2.0f);

    t3d_model_draw_material(obj->material, nullptr);
    rspq_block_run(obj->userBlock);
  }

  void MagicSpell::drawPTX(float deltaTime)
  {
    if(!checkFrustumAABB(
      pos - fm_vec3_t{15.0f, 5.0f,  15.0f},
      pos + fm_vec3_t{15.0f, 80.0f, 15.0f}
    ))return;

    // fake viewing angle by making particles less tall if the camera is looking down
    float camY = state.activeScene->getCam().getDirection().y;
    camY = fabsf(camY);

    tpx_state_set_scale(0.4f, 1.0f - (camY * 0.6f));
    particles.draw();
  }
}