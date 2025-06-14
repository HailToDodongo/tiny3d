/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

class Scene
{
  private:

  public:
    virtual void update(float deltaTime) = 0;
    virtual void draw(float deltaTime) = 0;
};