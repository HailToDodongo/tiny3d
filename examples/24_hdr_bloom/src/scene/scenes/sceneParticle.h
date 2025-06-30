/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../scene.h"
#include "../../camera/flyCam.h"
#include "../../render/ptSystem.h"
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

class SceneParticle : public Scene
{
  private:
    T3DModel *mapModel{};
    T3DMat4FP* mapMatFP{};
    float lightAngle{};

    FlyCam flyCam{camera};
    PTSystem particles{256};

    void updateScene(float deltaTime) final;
    void draw3D(float deltaTime) final;

  public:
    SceneParticle();
    ~SceneParticle();
};