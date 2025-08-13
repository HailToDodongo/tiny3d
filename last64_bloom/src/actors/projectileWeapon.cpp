/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "projectileWeapon.h"

namespace Actor {
    ProjectileWeapon::ProjectileWeapon() : Weapon() {
        // Initialize projectile pool
        Projectile::initialize();
        
        // Set weapon-specific properties
        fireRate = 0.1f;              // 10 shots per second
        projectileSpeed = 50.0f;
        projectileSlowdown = 0.0f;    // No slowdown
        maxUpgradeLevel = 5;
        spawnOffset = {0, 0, 0};
    }
    
    ProjectileWeapon::~ProjectileWeapon() {
        // Cleanup projectile pool
        Projectile::cleanup();
    }
    
    void ProjectileWeapon::update(float deltaTime) {
        // Update fire cooldown
        if (fireCooldown > 0) {
            fireCooldown -= deltaTime;
            if (fireCooldown < 0) {
                fireCooldown = 0;
            }
        }
    }
    
    void ProjectileWeapon::draw3D(float deltaTime) {
        // Draw all projectiles
        Projectile::drawAll(deltaTime);
    }
    
    void ProjectileWeapon::drawPTX(float deltaTime) {
        // No particle effects for this weapon
    }
    
    void ProjectileWeapon::fire(const T3DVec3& position, const T3DVec3& direction) {
        // Check if we can fire (cooldown)
        if (fireCooldown > 0) {
            return;
        }
        
        // Reset cooldown
        fireCooldown = fireRate;
        
        // Calculate spawn position with offset
        T3DVec3 spawnPos = {{
            position.x + spawnOffset.x,
            position.y + spawnOffset.y,
            position.z + spawnOffset.z
        }};
        
        // Spawn projectile
        Projectile::spawn(spawnPos, direction, projectileSpeed, projectileSlowdown);
    }
}