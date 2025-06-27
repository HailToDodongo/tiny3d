/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include <vector>
#include "../actors/base.h"
#include "../camera/camera.h"

class Scene
{
  private:

  protected:
    std::vector<Actor::Base*> actors{};
    Camera camera{};

  public:

    virtual ~Scene() {
      for(auto actor : actors) {
        delete actor;
      }
    }

    virtual void update(float deltaTime) = 0;
    virtual void draw(float deltaTime) = 0;

    Camera &getCam() { return camera; }
};