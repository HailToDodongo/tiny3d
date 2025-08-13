/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../actors/base.h"
#include <t3d/t3d.h>

namespace Actor {
    class Player : public Base {
    private:
        T3DVec3 position;
        T3DVec3 velocity;
        float speed;
        float rotation;
        
    public:
        Player(T3DVec3 startPos);
        void update(float deltaTime) override;
        void draw3D(float deltaTime) override;
        void drawPTX(float deltaTime) override;
        
        T3DVec3 getPosition() const { return position; }
        void setPosition(T3DVec3 newPos) { position = newPos; }
    };
}