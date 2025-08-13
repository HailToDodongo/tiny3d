/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "enemy.h"
#include <t3d/t3d.h>
#include <t3d/tpx.h>
#include <libdragon.h>
#include <malloc.h>

namespace Actor {
    // Static member definitions
    T3DVertPacked* Enemy::sharedVertices = nullptr;
    T3DMat4FP** Enemy::sharedMatrices = nullptr;
    bool* Enemy::activeFlags = nullptr;
    uint32_t Enemy::activeCount = 0;
    bool Enemy::initialized = false;
    Enemy Enemy::enemyPool[MAX_ENEMIES];

    Enemy::Enemy() : Base() {
        if (!initialized) {
            initializePool();
        }
        
        poolIndex = MAX_ENEMIES; // Invalid index until spawned
        position = {0, 0, 0};
        velocity = {0, 0, 0};
        speed = 0.0f;
        flags |= FLAG_DISABLED; // Start as disabled
    }

    Enemy::~Enemy() {
        // We don't actually delete from the pool here
        // The pool is managed statically
    }

    void Enemy::initialize() {
        if (!initialized) {
            initializePool();
        }
    }

    void Enemy::cleanup() {
        if (sharedVertices) {
            free_uncached(sharedVertices);
            sharedVertices = nullptr;
        }
        
        if (sharedMatrices) {
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (sharedMatrices[i]) {
                    free_uncached(sharedMatrices[i]);
                }
            }
            free(sharedMatrices);
            sharedMatrices = nullptr;
        }
        
        if (activeFlags) {
            free(activeFlags);
            activeFlags = nullptr;
        }
        
        activeCount = 0;
        initialized = false;
    }

    void Enemy::initializePool() {
        if (initialized) return;

        // Allocate vertices for all enemies (quads)
        T3DVec3 normalVec = {{0.0f, 0.0f, 1.0f}};
        uint16_t norm = t3d_vert_pack_normal(&normalVec);
        sharedVertices = (T3DVertPacked*)malloc_uncached(sizeof(T3DVertPacked) * MAX_ENEMIES * 2);
        
        for (int i = 0; i < MAX_ENEMIES; i++) {
            int idx = i * 2;
            // First structure: vertices 0 and 1
            sharedVertices[idx] = (T3DVertPacked){};
            sharedVertices[idx].posA[0] = -3; sharedVertices[idx].posA[1] = -3; sharedVertices[idx].posA[2] = 0;
            sharedVertices[idx].normA = norm;
            sharedVertices[idx].posB[0] = 3; sharedVertices[idx].posB[1] = -3; sharedVertices[idx].posB[2] = 0;
            sharedVertices[idx].normB = norm;
            // Ensure enemy colors are also bright for bloom effect
            sharedVertices[idx].rgbaA = 0xFFFF60FF; // Bright yellow
            sharedVertices[idx].rgbaB = 0xFFFF60FF; // Bright yellow
            sharedVertices[idx].stA[0] = 0; sharedVertices[idx].stA[1] = 0;
            sharedVertices[idx].stB[0] = 0; sharedVertices[idx].stB[1] = 0;
            
            // Second structure: vertices 2 and 3
            sharedVertices[idx+1] = (T3DVertPacked){};
            sharedVertices[idx+1].posA[0] = 3; sharedVertices[idx+1].posA[1] = 3; sharedVertices[idx+1].posA[2] = 0;
            sharedVertices[idx+1].normA = norm;
            sharedVertices[idx+1].posB[0] = -3; sharedVertices[idx+1].posB[1] = 3; sharedVertices[idx+1].posB[2] = 0;
            sharedVertices[idx+1].normB = norm;
            sharedVertices[idx+1].rgbaA = 0xFFFF60FF; // Bright yellow
            sharedVertices[idx+1].rgbaB = 0xFFFF60FF; // Bright yellow
            sharedVertices[idx+1].stA[0] = 0; sharedVertices[idx+1].stA[1] = 0;
            sharedVertices[idx+1].stB[0] = 0; sharedVertices[idx+1].stB[1] = 0;
        }
        
        // Allocate matrices for all enemies
        sharedMatrices = (T3DMat4FP**)malloc(sizeof(T3DMat4FP*) * MAX_ENEMIES);
        for (int i = 0; i < MAX_ENEMIES; i++) {
            sharedMatrices[i] = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
            t3d_mat4fp_identity(sharedMatrices[i]);
        }
        
        // Allocate active flags
        activeFlags = (bool*)calloc(MAX_ENEMIES, sizeof(bool));
        
        initialized = true;
    }

    Enemy* Enemy::spawn(const T3DVec3& position, const T3DVec3& velocity, float speed) {
        if (!initialized) {
            initializePool();
        }
        
        // Find an inactive enemy slot
        for (uint32_t i = 0; i < MAX_ENEMIES; i++) {
            if (!activeFlags[i]) {
                // Found an inactive slot, activate it
                activeFlags[i] = true;
                activeCount++;
                
                // Get an enemy from the pool
                Enemy* enemy = &enemyPool[i];
                
                enemy->poolIndex = i;
                enemy->position = position;
                enemy->velocity = velocity;
                enemy->speed = speed;
                enemy->flags &= ~FLAG_DISABLED; // Enable the enemy
                
                return enemy;
            }
        }
        
        // No inactive slots available
        return nullptr;
    }

    void Enemy::updateAll(float deltaTime) {
        if (!initialized) return;
        
        for (uint32_t i = 0; i < MAX_ENEMIES; i++) {
            if (activeFlags[i]) {
                Enemy* enemy = &enemyPool[i];
                enemy->update(deltaTime);
            }
        }
    }

    void Enemy::drawAll(float deltaTime) {
        if (!initialized) return;
        
        // Set up rendering state once for all enemies
        t3d_state_set_drawflags((enum T3DDrawFlags)(T3D_FLAG_SHADED | T3D_FLAG_DEPTH));
        
        for (uint32_t i = 0; i < MAX_ENEMIES; i++) {
            if (activeFlags[i]) {
                Enemy* enemy = &enemyPool[i];
                enemy->draw3D(deltaTime);
            }
        }
    }

    void Enemy::update(float deltaTime) {
        if (flags & FLAG_DISABLED) return;
        
        // Move enemy
        position.x += velocity.x * speed * deltaTime;
        position.y += velocity.y * speed * deltaTime;
        position.z += velocity.z * speed * deltaTime;
        
        // Deactivate enemies that go off-screen
        if (position.x < -40 || position.x > 40 || position.y < -40 || position.y > 40) {
            deactivate();
            return;
        }
        
        // Update matrix
        if (poolIndex < MAX_ENEMIES) {
            t3d_mat4fp_from_srt_euler(
                sharedMatrices[poolIndex],
                (T3DVec3){{1.0f, 1.0f, 1.0f}},  // scale
                (T3DVec3){{0.0f, 0.0f, 0.0f}},  // rotation
                position                         // translation
            );
        }
    }

    void Enemy::draw3D(float deltaTime) {
        if (flags & FLAG_DISABLED) return;
        
        if (poolIndex < MAX_ENEMIES) {
            t3d_matrix_push(sharedMatrices[poolIndex]);
            t3d_vert_load(&sharedVertices[poolIndex * 2], 0, 4); // Load 4 vertices (2 structures)
            t3d_tri_draw(0, 1, 2);
            t3d_tri_draw(2, 3, 0);
            t3d_tri_sync();
            t3d_matrix_pop(1);
        }
    }

    void Enemy::drawPTX(float deltaTime) {
        // No particle effects for enemies
    }

    void Enemy::deactivate() {
        if (poolIndex < MAX_ENEMIES) {
            activeFlags[poolIndex] = false;
            flags |= FLAG_DISABLED;
            activeCount--;
        }
    }

    bool Enemy::isActive() const {
        if (poolIndex < MAX_ENEMIES) {
            return activeFlags[poolIndex] && !(flags & FLAG_DISABLED);
        }
        return false;
    }
}