/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../scene.h"
#include "../../camera/camera.h"
#include "../../camera/staticCam.h"
#include "../../render/debugDraw.h"
#include "../../actors/player.h" // Include Player class
#include "../../actors/enemy.h"  // Include Enemy class
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>

class SceneLast64 : public Scene
{
  private:
    // Player instance
    Actor::Player* player;
    
    // Projectiles
    float projX[20] = {0};
    float projY[20] = {0};
    bool projActive[20] = {false};
    
    // Vertices for geometric shapes
    // Removed playerVertices as it's now handled by the Player class
    T3DVertPacked* projVertices;
    
    // Matrices
    // Removed playerMat and playerMatFP as they're now handled by the Player class
    T3DMat4 projMat[20];
    T3DMat4FP* projMatFP[20];
    
    StaticCam staticCam{camera};
    
    void updateScene(float deltaTime) final;
    void draw3D(float deltaTime) final;

  public:
    void draw2D(float deltaTime) final;

    SceneLast64();
    ~SceneLast64();
};