/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../actors/base.h"
#include "../actors/player.h"
#include <t3d/t3d.h>

#define MAX_PROJECTILES 100

namespace Actor {
    // Forward declarations
    class Projectile;
    class Player;
    
    class Weapon : public Base {
    protected:
        static Player* player;        // Reference to the player
        float fireCooldown;           // Current cooldown timer
        float fireRate;              // Time between shots (seconds)
        float projectileSpeed;       // Speed of projectiles
        float projectileSlowdown;    // Slowdown factor per second
        int upgradeLevel;            // Current upgrade level
        int maxUpgradeLevel;         // Maximum upgrade level
        T3DVec3 spawnOffset;         // Offset from player position
        
    public:
        Weapon();
        ~Weapon();
        
        virtual void update(float deltaTime) override = 0;
        virtual void draw3D(float deltaTime) override = 0;
        virtual void drawPTX(float deltaTime) override = 0;
        
        virtual void fire(const T3DVec3& position, const T3DVec3& direction);
        virtual void upgrade() { if (upgradeLevel < maxUpgradeLevel) upgradeLevel++; }
        
        // Getters and setters
        static void setPlayer(Player* p) { player = p; }
        static Player* getPlayer() { return player; }
        
        float getFireRate() const { return fireRate; }
        void setFireRate(float rate) { fireRate = rate; }
        
        float getProjectileSpeed() const { return projectileSpeed; }
        void setProjectileSpeed(float speed) { projectileSpeed = speed; }
        
        int getUpgradeLevel() const { return upgradeLevel; }
        int getMaxUpgradeLevel() const { return maxUpgradeLevel; }
        
        T3DVec3 getSpawnOffset() const { return spawnOffset; }
        void setSpawnOffset(const T3DVec3& offset) { spawnOffset = offset; }
    };
}