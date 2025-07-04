/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "scene.h"
#include "../main.h"

void Scene::update(float deltaTime)
{
  updateScene(deltaTime);
  camera.update(deltaTime);

  for(auto actor : actors) {
    if(!(actor->flags & Actor::Base::FLAG_DISABLED)) {
      actor->update(deltaTime);
    }
  }
}

void Scene::draw(float deltaTime)
{
  camera.attach();

  draw3D(deltaTime);

  t3d_matrix_push_pos(1);
  for(auto actor : actors) {
    if(!(actor->flags & Actor::Base::FLAG_DISABLED)) {
      actor->draw3D(deltaTime);
    }
  }
  t3d_matrix_pop(1);
}