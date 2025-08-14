/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "player.h"
#include "projectileWeapon.h"
#include <t3d/t3d.h>
#include <t3d/tpx.h>
#include <libdragon.h>
#include <malloc.h>

// Screen boundaries
static constexpr float SCREEN_LEFT = 0.0f;
static constexpr float SCREEN_RIGHT = 312.0f;
static constexpr float SCREEN_TOP = 0.0f;
static constexpr float SCREEN_BOTTOM = 232.0f;

namespace Actor {
    // Static members for player mesh
    T3DVertPacked* Player::sharedVertices = nullptr;
    T3DMat4FP* Player::sharedMatrix = nullptr;
    bool Player::initialized = false;
    
    // Static member definition
    Player* Player::instance = nullptr;
    
    void Player::initialize() {
        if (initialized) return;
        
        // Allocate vertices for the player (triangle)
        T3DVec3 normalVec = {{0.0f, 0.0f, 1.0f}};
        uint16_t norm = t3d_vert_pack_normal(&normalVec);
        sharedVertices = (T3DVertPacked*)malloc_uncached(sizeof(T3DVertPacked) * 2);
        
        // First structure: vertices 0 and 1
        sharedVertices[0] = (T3DVertPacked){};
        sharedVertices[0].posA[0] = 0; sharedVertices[0].posA[1] = 3; sharedVertices[0].posA[2] = 0; // Top vertex (smaller to match enemies)
        sharedVertices[0].normA = norm;
        sharedVertices[0].posB[0] = -3; sharedVertices[0].posB[1] = -3; sharedVertices[0].posB[2] = 0; // Bottom left
        sharedVertices[0].normB = norm;
        sharedVertices[0].rgbaA = 0xFFFF0000; // Will be set per player
        sharedVertices[0].rgbaB = 0xFFFF0000; // Will be set per player
        sharedVertices[0].stA[0] = 0; sharedVertices[0].stA[1] = 0;
        sharedVertices[0].stB[0] = 0; sharedVertices[0].stB[1] = 0;
        
        // Second structure: vertex 2 and unused
        sharedVertices[1] = (T3DVertPacked){};
        sharedVertices[1].posA[0] = 3; sharedVertices[1].posA[1] = -3; sharedVertices[1].posA[2] = 0; // Bottom right
        sharedVertices[1].normA = norm;
        sharedVertices[1].posB[0] = 0; sharedVertices[1].posB[1] = 0; sharedVertices[1].posB[2] = 0; // Unused
        sharedVertices[1].normB = norm;
        sharedVertices[1].rgbaA = 0xFFFF0000; // Will be set per player
        sharedVertices[1].rgbaB = 0xFFFF0000; // Will be set per player
        sharedVertices[1].stA[0] = 0; sharedVertices[1].stA[1] = 0;
        sharedVertices[1].stB[0] = 0; sharedVertices[1].stB[1] = 0;
        
        // Allocate matrix
        sharedMatrix = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
        t3d_mat4fp_identity(sharedMatrix);
        
        initialized = true;
    }
    
    void Player::cleanup() {
        if (sharedVertices) {
            free_uncached(sharedVertices);
            sharedVertices = nullptr;
        }
        
        if (sharedMatrix) {
            free_uncached(sharedMatrix);
            sharedMatrix = nullptr;
        }
        
        initialized = false;
    }
    
    Player::Player(T3DVec3 startPos, joypad_port_t port) : Base() {
        if (!initialized) {
            initialize();
        }
        
        instance = this; // Set the instance pointer
        position = startPos;
        velocity = {0, 0, 0};
        speed = 50.0f;
        rotation = 0.0f;
        playerPort = port;
        
        // Set player color based on port
        if (sharedVertices) {
            uint32_t color = (playerPort == JOYPAD_PORT_1) ? 0xFF1980FF : 0xFFFF1980; // Blue or Red
            sharedVertices[0].rgbaA = color;
            sharedVertices[0].rgbaB = color;
            sharedVertices[1].rgbaA = color;
            sharedVertices[1].rgbaB = color;
        }
        
        // Initialize weapon
        weapon = new ProjectileWeapon();
        weapon->setPlayer(this); // Set the player reference in the weapon
        
        flags &= ~FLAG_DISABLED; // Clear the disabled flag to enable the actor
    }
    
    Player::~Player() {
        if (instance == this) {
            instance = nullptr; // Clear the instance pointer when destroyed
        }
        
        // Clean up weapon
        if (weapon) {
            delete weapon;
            weapon = nullptr;
        }
    }
    
    void Player::update(float deltaTime) {
        // Handle player input
        joypad_buttons_t held = joypad_get_buttons_held(playerPort);
        joypad_inputs_t stick = joypad_get_inputs(playerPort); // Get analog stick inputs
        
        float moveSpeed = speed * deltaTime;
        
        // 3D movement - X and Y for horizontal/vertical movement, Z stays constant
        // Note: In the N64's coordinate system with the camera looking down the Z-axis,
        // positive Y is up, negative Y is down, positive X is right, negative X is left
        // 
        // Using analog stick for smooth movement in all directions
        // Invert Y axis to match typical 2D game conventions (up is positive)
        float moveX = stick.stick_x / 32.0f; // Normalize analog input (-1 to 1)
        float moveY = stick.stick_y / 32.0f; // normalize
        
        // Apply deadzone to prevent drift
        static constexpr float DEADZONE = 0.2f; // 20% deadzone
        if (fabsf(moveX) < DEADZONE) moveX = 0.0f;
        if (fabsf(moveY) < DEADZONE) moveY = 0.0f;
        
        // Apply movement
        float newX = position.x + moveX * moveSpeed;
        float newY = position.y + moveY * moveSpeed;
        
        // Check boundaries
        if (newX >= SCREEN_LEFT && newX <= SCREEN_RIGHT) {
            position.x = newX;
        }
        if (newY >= SCREEN_TOP && newY <= SCREEN_BOTTOM) {
            position.y = newY;
        }
        // Z position stays constant (we're moving on the X/Y plane)
        
        // Update weapon
        if (weapon) {
            weapon->update(deltaTime);
        }
        
        // Check if A button is pressed for firing
        joypad_buttons_t pressed = joypad_get_buttons_pressed(playerPort);
        if (pressed.a) {
            // Fire in the direction the player is facing (based on rotation)
            float fireX = sinf(rotation);
            float fireY = cosf(rotation);
            weapon->fire(position, {{fireX, fireY, 0.0f}});
        }
        
        // Update rotation based on movement direction
        if (moveX != 0.0f || moveY != 0.0f) {
            rotation = atan2f(moveX, moveY); // Point in movement direction
        }
        
        // Update matrix
        if (sharedMatrix) {
            t3d_mat4fp_from_srt_euler(
                sharedMatrix,
                (T3DVec3){{1.0f, 1.0f, 1.0f}},  // scale
                (T3DVec3){{0.0f, 0.0f, rotation}},  // rotation around Z axis
                position                         // translation
            );
        }
    }
    
    void Player::draw3D(float deltaTime) {
    // Set up rendering state
    t3d_state_set_drawflags((enum T3DDrawFlags)(T3D_FLAG_SHADED | T3D_FLAG_DEPTH));
    
    // Draw the player using the shared vertices and matrix
    if (sharedMatrix && sharedVertices) {
        t3d_matrix_push(sharedMatrix);
        t3d_vert_load(sharedVertices, 0, 4); // Load 4 vertices (2 structures)
        t3d_tri_draw(0, 1, 2); // Draw triangle with vertices 0, 1, 2
        t3d_tri_sync();
        t3d_matrix_pop(1);
    }
    
    // Draw weapon projectiles
    if (weapon) {
        weapon->draw3D(deltaTime);
    }
}
    
    void Player::drawPTX(float deltaTime) {
        // Draw weapon particle effects
        if (weapon) {
            weapon->drawPTX(deltaTime);
        }
    }
}