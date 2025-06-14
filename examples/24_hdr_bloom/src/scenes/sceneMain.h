/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "scene.h"
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

class SceneMain : public Scene
{
  private:
    T3DModel *mapModel{};
    T3DMat4FP* mapMatFP{};
    float lightAngle{};

  public:
    SceneMain();
    ~SceneMain();
    void update(float deltaTime) final;
    void draw(float deltaTime) final;
};