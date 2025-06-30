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

    virtual void updateScene(float deltaTime) = 0;
    virtual void draw3D(float deltaTime) = 0;
    virtual void drawPostHDR(float deltaTime) {}

  public:

    virtual ~Scene() {
      for(auto actor : actors) {
        delete actor;
      }
    }

    void update(float deltaTime);
    void draw(float deltaTime);

    Camera &getCam() { return camera; }
};