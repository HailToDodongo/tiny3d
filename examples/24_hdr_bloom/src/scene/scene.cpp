/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "scene.h"
#include "../main.h"
#include <t3d/tpx.h>

void Scene::update(float deltaTime)
{
  updateScene(deltaTime);
  camera.update(deltaTime);
  camera.attach();

  for(auto actor : actors) {
    if(!(actor->flags & Actor::Base::FLAG_DISABLED)) {
      actor->update(deltaTime);
    }
  }
}

void Scene::draw(float deltaTime)
{
  draw3D(deltaTime);

  t3d_matrix_push_pos(1);
  for(auto actor : actors) {
    if(!(actor->flags & Actor::Base::FLAG_DISABLED)) {
      actor->draw3D(deltaTime);
    }
  }
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
  tpx_state_set_scale(0.5f, 0.5f);
  for(auto actor : actors) {
    if(!(actor->flags & Actor::Base::FLAG_DISABLED)) {
      actor->drawPTX(deltaTime);
    }
  }
}