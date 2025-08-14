/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "projectileWeapon.h"
#include <libdragon.h>

namespace Actor {
    ProjectileWeapon::ProjectileWeapon() : Weapon() {
        // Initialize projectile pool
        Projectile::initialize();
        
        // Set weapon-specific properties
        fireRate = 0.3f;              // Reduced to ~3 shots per second
        projectileSpeed = 2.0f;
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
        
        // Auto-fire logic
        if (fireCooldown <= 0 && player) {
            // Reset cooldown
            fireCooldown = fireRate;
            
            // Get player position
            T3DVec3 playerPos = player->getPosition();
            
            // Fire weapon in a fixed direction (upwards for now)
            T3DVec3 direction = {{0.0f, 1.0f, 0.0f}};
            fire(playerPos, direction);
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