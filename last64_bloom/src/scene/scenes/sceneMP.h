/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#ifndef SCENE_MP_H
#define SCENE_MP_H

#include "../scene.h"
#include "../../actors/player.h"
#include "../../actors/enemy.h"
#include "../../actors/projectile.h"

class SceneMP : public Scene {
public:
    SceneMP();
    ~SceneMP() override;

    void updateScene(float deltaTime) override;
    void draw3D(float deltaTime) override;
    void draw2D(float deltaTime) override;

private:
    Actor::Player* player1;
    Actor::Player* player2;
};

#endif // SCENE_MP_H