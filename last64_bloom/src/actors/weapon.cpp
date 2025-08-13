/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "weapon.h"

namespace Actor {
    Weapon::Weapon() : Base() {
        fireCooldown = 0.0f;
        fireRate = 0.1f;           // 10 shots per second
        projectileSpeed = 50.0f;
        projectileSlowdown = 0.0f; // No slowdown by default
        upgradeLevel = 1;
        maxUpgradeLevel = 5;
        spawnOffset = {0, 0, 0};
    }
    
    Weapon::~Weapon() {
        // Virtual destructor
    }
}