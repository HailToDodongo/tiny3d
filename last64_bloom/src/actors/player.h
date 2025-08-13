/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../actors/base.h"
#include "../actors/projectile.h"
#include <t3d/t3d.h>
#include <libdragon.h>

namespace Actor {
    class Player : public Base {
    private:
        static Player* instance;
        T3DVec3 position;
        T3DVec3 velocity;
        float speed;
        float rotation;
        
        // Weapon properties
        float fireCooldown;
        float fireRate;
        float projectileSpeed;
        joypad_port_t playerPort; // To identify which controller this player uses
        
    public:
        Player(T3DVec3 startPos, joypad_port_t port);
        ~Player();
        void update(float deltaTime) override;
        void draw3D(float deltaTime) override;
        void drawPTX(float deltaTime) override;
        
        static Player* getInstance() { return instance; }
        T3DVec3 getPosition() const { return position; }
        void setPosition(T3DVec3 newPos) { position = newPos; }
        
        // Weapon methods
        void fire();
    };
}