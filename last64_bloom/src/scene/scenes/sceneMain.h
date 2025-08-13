/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../scene.h"
#include "../../camera/camera.h"
#include "../../camera/staticCam.h"
#include "../../camera/flyCam.h"
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

class SceneMain : public Scene
{
  private:
    T3DModel *mapModel{};
    T3DMat4FP* mapMatFP{};
    T3DMat4FP* skyMatFP{};

    StaticCam staticCam{camera};
    FlyCam flyCam{camera};

    void updateScene(float deltaTime) final;
    void draw3D(float deltaTime) final;

  public:
    bool useFlyCam{true};
    
    void draw2D(float deltaTime) final;

    SceneMain();
    ~SceneMain();
};