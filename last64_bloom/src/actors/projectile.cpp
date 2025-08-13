/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "projectile.h"
#include <t3d/t3d.h>
#include <t3d/tpx.h>
#include <libdragon.h>
#include <malloc.h>

// Screen boundaries
static constexpr float SCREEN_LEFT = 0.0f;
static constexpr float SCREEN_RIGHT = 312.0f;
static constexpr float SCREEN_TOP = 0.0f;
static constexpr float SCREEN_BOTTOM = 236.0f;

namespace Actor {
    // Static member definitions
    T3DVertPacked* Projectile::sharedVertices = nullptr;
    T3DMat4FP** Projectile::sharedMatrices = nullptr;
    bool* Projectile::activeFlags = nullptr;
    uint32_t Projectile::activeCount = 0;
    bool Projectile::initialized = false;
    Projectile Projectile::projectilePool[MAX_PROJECTILES];
    
    Projectile::Projectile() : Base() {
        if (!initialized) {
            initializePool();
        }
        
        poolIndex = MAX_PROJECTILES; // Invalid index until spawned
        position = {0, 0, 0};
        velocity = {0, 0, 0};
        speed = 0.0f;
        slowdown = 0.0f;
        lifetime = 0.0f;
        maxLifetime = 10.0f;  // Increased to 10 seconds lifetime
        flags |= FLAG_DISABLED; // Start as disabled
    }
    
    Projectile::~Projectile() {
        // We don't actually delete from the pool here
        // The pool is managed statically
    }
    
    void Projectile::initialize() {
        if (!initialized) {
            initializePool();
        }
    }
    
    void Projectile::cleanup() {
        if (sharedVertices) {
            free_uncached(sharedVertices);
            sharedVertices = nullptr;
        }
        
        if (sharedMatrices) {
            for (int i = 0; i < MAX_PROJECTILES; i++) {
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
    
    void Projectile::initializePool() {
        if (initialized) return;
        
        // Allocate vertices for all projectiles (thin quads)
        T3DVec3 normalVec = {{0.0f, 0.0f, 1.0f}};
        uint16_t norm = t3d_vert_pack_normal(&normalVec);
        sharedVertices = (T3DVertPacked*)malloc_uncached(sizeof(T3DVertPacked) * MAX_PROJECTILES * 2);
        
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            int idx = i * 2;
            // Using a thin quad to represent a line
            // First structure: vertices 0 and 1
            sharedVertices[idx] = (T3DVertPacked){};
            sharedVertices[idx].posA[0] = -1; sharedVertices[idx].posA[1] = -0.5; sharedVertices[idx].posA[2] = 0;
            sharedVertices[idx].normA = norm;
            sharedVertices[idx].posB[0] = 1; sharedVertices[idx].posB[1] = -0.5; sharedVertices[idx].posB[2] = 0;
            sharedVertices[idx].normB = norm;
            // Projectile colors are now bright purple
            sharedVertices[idx].rgbaA = 0xFF00FFFF; // Bright purple (red + blue)
            sharedVertices[idx].rgbaB = 0xFF00FFFF; // Bright purple (red + blue)
            sharedVertices[idx].stA[0] = 0; sharedVertices[idx].stA[1] = 0;
            sharedVertices[idx].stB[0] = 0; sharedVertices[idx].stB[1] = 0;
            
            // Second structure: vertices 2 and 3 (completing the quad)
            sharedVertices[idx+1] = (T3DVertPacked){};
            sharedVertices[idx+1].posA[0] = 1; sharedVertices[idx+1].posA[1] = 0.5; sharedVertices[idx+1].posA[2] = 0;
            sharedVertices[idx+1].normA = norm;
            sharedVertices[idx+1].posB[0] = -1; sharedVertices[idx+1].posB[1] = 0.5; sharedVertices[idx+1].posB[2] = 0;
            sharedVertices[idx+1].normB = norm;
            sharedVertices[idx+1].rgbaA = 0xFF00FFFF; // Bright purple (red + blue)
            sharedVertices[idx+1].rgbaB = 0xFF00FFFF; // Bright purple (red + blue)
            sharedVertices[idx+1].stA[0] = 0; sharedVertices[idx+1].stA[1] = 0;
            sharedVertices[idx+1].stB[0] = 0; sharedVertices[idx+1].stB[1] = 0;
        }
        
        // Allocate matrices for all projectiles
        sharedMatrices = (T3DMat4FP**)malloc(sizeof(T3DMat4FP*) * MAX_PROJECTILES);
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            sharedMatrices[i] = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
            t3d_mat4fp_identity(sharedMatrices[i]);
        }
        
        // Allocate active flags
        activeFlags = (bool*)calloc(MAX_PROJECTILES, sizeof(bool));
        
        initialized = true;
    }
    
    Projectile* Projectile::spawn(const T3DVec3& position, const T3DVec3& velocity, float speed, float slowdown) {
        if (!initialized) {
            initializePool();
        }
        
        // Find an inactive projectile slot
        for (uint32_t i = 0; i < MAX_PROJECTILES; i++) {
            if (!activeFlags[i]) {
                // Found an inactive slot, activate it
                activeFlags[i] = true;
                activeCount++;
                
                // Get a projectile from the pool
                Projectile* projectile = &projectilePool[i];
                
                projectile->poolIndex = i;
                projectile->position = position;
                projectile->velocity = velocity;
                projectile->speed = speed;
                projectile->slowdown = slowdown;
                projectile->lifetime = 0.0f;  // Reset lifetime
                projectile->flags &= ~FLAG_DISABLED; // Enable the projectile
                
                return projectile;
            }
        }
        
        // No inactive slots available
        return nullptr;
    }
    
    void Projectile::updateAll(float deltaTime) {
        if (!initialized) return;
        
        for (uint32_t i = 0; i < MAX_PROJECTILES; i++) {
            if (activeFlags[i]) {
                Projectile* projectile = &projectilePool[i];
                projectile->update(deltaTime);
            }
        }
    }
    
    void Projectile::drawAll(float deltaTime) {
        if (!initialized) return;
        
        // Set up rendering state once for all projectiles
        t3d_state_set_drawflags((enum T3DDrawFlags)(T3D_FLAG_SHADED | T3D_FLAG_DEPTH));
        
        for (uint32_t i = 0; i < MAX_PROJECTILES; i++) {
            if (activeFlags[i]) {
                Projectile* projectile = &projectilePool[i];
                projectile->draw3D(deltaTime);
            }
        }
    }

    void Projectile::update(float deltaTime) {
        if (flags & FLAG_DISABLED) return;
        
        // Update lifetime
        lifetime += deltaTime;
        if (lifetime >= maxLifetime) {
            deactivate();
            return;
        }
        
        // Apply slowdown
        if (slowdown > 0.0f) {
            float speedReduction = slowdown * deltaTime;
            float currentSpeed = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
            
            if (currentSpeed > speedReduction) {
                float factor = (currentSpeed - speedReduction) / currentSpeed;
                velocity.x *= factor;
                velocity.y *= factor;
                velocity.z *= factor;
            } else {
                velocity.x = 0;
                velocity.y = 0;
                velocity.z = 0;
            }
        }
        
        // Move projectile
        position.x += velocity.x * speed * deltaTime;
        position.y += velocity.y * speed * deltaTime;
        position.z += velocity.z * speed * deltaTime;
        
        // Deactivate projectiles that go off-screen
        if (position.x < SCREEN_LEFT || position.x > SCREEN_RIGHT || 
            position.y < SCREEN_TOP || position.y > SCREEN_BOTTOM) {
            deactivate();
            return;
        }
        
        // Update matrix
        if (poolIndex < MAX_PROJECTILES) {
            t3d_mat4fp_from_srt_euler(
                sharedMatrices[poolIndex],
                (T3DVec3){{1.0f, 1.0f, 1.0f}},  // scale
                (T3DVec3){{0.0f, 0.0f, 0.0f}},  // rotation
                position                         // translation
            );
        }
    }
    
    void Projectile::draw3D(float deltaTime) {
        if (flags & FLAG_DISABLED) return;
        
        if (poolIndex < MAX_PROJECTILES) {
            t3d_matrix_push(sharedMatrices[poolIndex]);
            t3d_vert_load(&sharedVertices[poolIndex * 2], 0, 4); // Load 4 vertices (2 structures)
            t3d_tri_draw(0, 1, 2);
            t3d_tri_draw(2, 3, 0);
            t3d_tri_sync();
            t3d_matrix_pop(1);
        }
    }
    
    void Projectile::drawPTX(float deltaTime) {
        // No particle effects for projectiles
    }
    
    void Projectile::deactivate() {
        if (poolIndex < MAX_PROJECTILES) {
            activeFlags[poolIndex] = false;
            flags |= FLAG_DISABLED;
            activeCount--;
        }
    }
    
    bool Projectile::isActive() const {
        if (poolIndex < MAX_PROJECTILES) {
            return activeFlags[poolIndex] && !(flags & FLAG_DISABLED);
        }
        return false;
    }
}