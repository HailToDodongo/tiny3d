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

class SceneEnv : public Scene
{
  private:
    sprite_t *spriteBG{};
    rspq_block_t *dplBG{};
    uint32_t bgIndex{0};
    bool showBlurred{true};
    bool showModel{true};

    void refreshScene();

    void updateScene(float deltaTime) final;
    void draw3D(float deltaTime) final;

  public:
    SceneEnv();
    ~SceneEnv();
};