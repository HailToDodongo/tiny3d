/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../scene.h"
#include "../../camera/camera.h"
#include "../../camera/flyCam.h"
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "../../memory/matrixManager.h"

class SceneSpace : public Scene
{
  private:
    T3DModel *model{};
    T3DObject *objSky{};

    RingMat4FP matFP{};
    RingMat4FP matFPSky{};

    FlyCam flyCam{camera};
    float timerSky{};

    void updateScene(float deltaTime) final;
    void draw3D(float deltaTime) final;

  public:
    SceneSpace();
    ~SceneSpace();
};