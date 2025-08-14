/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../actors/base.h"
#include "../actors/projectile.h"
#include "../actors/weapon.h"
#include <t3d/t3d.h>
#include <libdragon.h>

namespace Actor {
    class Player : public Base {
    private:
        static T3DVertPacked* sharedVertices;
        static T3DMat4FP* sharedMatrix;
        static bool initialized;
        
        T3DVertPacked* playerVertices; // Per-player vertices
        T3DMat4FP* playerMatrix; // Per-player matrix
        
        T3DVec3 position;
        T3DVec3 velocity;
        float speed;
        float rotation;
        joypad_port_t playerPort; // To identify which controller this player uses
        uint32_t playerColor; // Store player-specific color
        
        // Weapon reference
        Weapon* weapon;
        
        static void initialize();
        static void cleanup();
        
    public:
        Player(T3DVec3 startPos, joypad_port_t port);
        ~Player();
        void update(float deltaTime) override;
        void draw3D(float deltaTime) override;
        void drawPTX(float deltaTime) override;
        
        T3DVec3 getPosition() const { return position; }
        void setPosition(T3DVec3 newPos) { position = newPos; }
        float getRotation() const { return rotation; }
        
        // Weapon methods
        Weapon* getWeapon() const { return weapon; }
        void setWeapon(Weapon* newWeapon) { weapon = newWeapon; }
        
        static void initializePlayer() { initialize(); }
        static void cleanupPlayer() { cleanup(); }
    };
}