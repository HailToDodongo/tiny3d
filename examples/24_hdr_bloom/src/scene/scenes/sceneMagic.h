/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../scene.h"
#include "../../camera/flyCam.h"
#include "../../render/ptSystem.h"
#include "../../memory/matrixManager.h"
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

class SceneMagic : public Scene
{
  private:
    T3DModel *mapModel{};
    T3DObject *objSky{};

    T3DMat4FP* mapMatFP{};
    RingMat4FP matFPSky{};

    float lightAngle{};

    FlyCam flyCam{camera};

    void updateScene(float deltaTime) final;
    void draw3D(float deltaTime) final;

  public:
    SceneMagic();
    ~SceneMagic();
};