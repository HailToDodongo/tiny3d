/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../actors/base.h"
#include <t3d/t3d.h>

#define MAX_PROJECTILES 100

namespace Actor {
    class Projectile : public Base {
    private:
        static T3DVertPacked* sharedVertices;
        static T3DMat4FP** sharedMatrices;
        static bool* activeFlags;
        static uint32_t activeCount;
        static bool initialized;
        static Projectile projectilePool[MAX_PROJECTILES];
        
        T3DVec3 position;
        T3DVec3 velocity;
        float speed;
        float slowdown;
        float lifetime;          // Time the projectile has been active
        float maxLifetime;       // Maximum time the projectile can be active
        uint32_t poolIndex;
        
        static void initializePool();
        
    public:
        Projectile();
        ~Projectile();
        
        static void initialize();
        static void cleanup();
        static Projectile* spawn(const T3DVec3& position, const T3DVec3& velocity, float speed, float slowdown);
        static void updateAll(float deltaTime);
        static void drawAll(float deltaTime);
        static uint32_t getActiveCount() { return activeCount; }
        
        void update(float deltaTime) override;
        void draw3D(float deltaTime) override;
        void drawPTX(float deltaTime) override;
        
        void deactivate();
        bool isActive() const;
        
        T3DVec3 getPosition() const { return position; }
        void setPosition(const T3DVec3& newPos) { position = newPos; }
        void setVelocity(const T3DVec3& newVelocity) { velocity = newVelocity; }
        
        float getMaxLifetime() const { return maxLifetime; }
        void setMaxLifetime(float lifetime) { maxLifetime = lifetime; }
    };
}