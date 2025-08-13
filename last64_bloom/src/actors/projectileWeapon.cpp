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
        fireRate = 0.1f;              // 10 shots per second
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
        
        // Manual fire logic (check for A button press)
        if (player) {
            // Check if A button is pressed
            joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
            
            if (pressed.a && fireCooldown <= 0) {
                // Reset cooldown
                fireCooldown = fireRate;
                
                // Get player position
                T3DVec3 playerPos = player->getPosition();
                
                // Fire weapon in a fixed direction (upwards for now)
                T3DVec3 direction = {{0.0f, 1.0f, 0.0f}};
                fire(playerPos, direction);
            }
        }
        
        // Auto-fire logic (moved from scene)
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