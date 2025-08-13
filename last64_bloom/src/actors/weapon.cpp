/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "weapon.h"

namespace Actor {
    // Static member definition
    Player* Weapon::player = nullptr;
    
    Weapon::Weapon() : Base() {
        fireCooldown = 0.0f;
        fireRate = 0.1f;              // 1 shots per second
        projectileSpeed = 100.0f;
        projectileSlowdown = 0.0f;
        upgradeLevel = 0;
        maxUpgradeLevel = 3;
        spawnOffset = {0, 0, 0};
        flags &= ~FLAG_DISABLED; // Clear the disabled flag to enable the actor
    }
    
    Weapon::~Weapon() {
        // Nothing to do here
    }
    
    void Weapon::fire(const T3DVec3& position, const T3DVec3& direction) {
        // Default implementation does nothing
        // This should be overridden by subclasses
    }
}