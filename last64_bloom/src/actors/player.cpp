/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "player.h"
#include <t3d/t3d.h>
#include <t3d/tpx.h>
#include <libdragon.h>

// Screen boundaries
static constexpr float SCREEN_LEFT = 0.0f;
static constexpr float SCREEN_RIGHT = 312.0f;
static constexpr float SCREEN_TOP = 0.0f;
static constexpr float SCREEN_BOTTOM = 236.0f;

namespace Actor {
    // Static member definition
    Player* Player::instance = nullptr;
    
    Player::Player(T3DVec3 startPos, joypad_port_t port) : Base() {
        instance = this; // Set the instance pointer
        position = startPos;
        velocity = {0, 0, 0};
        speed = 100.0f;
        rotation = 0.0f;
        playerPort = port;
        
        // Initialize weapon properties
        fireCooldown = 0.0f;
        fireRate = 0.3f;              // Reduced to ~3 shots per second
        projectileSpeed = 2.0f;
        
        flags &= ~FLAG_DISABLED; // Clear the disabled flag to enable the actor
    }
    
    Player::~Player() {
        if (instance == this) {
            instance = nullptr; // Clear the instance pointer when destroyed
        }
    }
    
    void Player::update(float deltaTime) {
        // Handle player input
        joypad_buttons_t held = joypad_get_buttons_held(playerPort);
        joypad_inputs_t stick = joypad_get_inputs(playerPort); // Get analog stick inputs
        
        float moveSpeed = speed * deltaTime;
        
        // Basic 2D movement on X/Y plane to match the scene's perspective
        // Note: In the N64's coordinate system with the camera looking down the Z-axis,
        // positive Y is up, negative Y is down, positive X is right, negative X is left
        // 
        // Using analog stick for smooth movement in all directions
        // Invert Y axis to match typical 2D game conventions (up is positive)
        float moveX = stick.stick_x / 32.0f; // Normalize analog input (-1 to 1)
        float moveY = -stick.stick_y / 32.0f; // Invert Y axis and normalize
        
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
        
        // Update fire cooldown
        if (fireCooldown > 0) {
            fireCooldown -= deltaTime;
            if (fireCooldown < 0) {
                fireCooldown = 0;
            }
        }
        
        // Check if A button is pressed for firing
        joypad_buttons_t pressed = joypad_get_buttons_pressed(playerPort);
        if (pressed.a && fireCooldown <= 0) {
            fire();
        }
        
        // For now, just update rotation to show we're alive
        rotation += deltaTime * 4.0f; // Rotate a bit faster
    }
    
    void Player::fire() {
        // Reset cooldown
        fireCooldown = fireRate;
        
        // Fire weapon in a fixed direction (upwards for now)
        T3DVec3 direction = {{0.0f, 1.0f, 0.0f}};
        
        // Spawn projectile
        Projectile::spawn(position, direction, projectileSpeed, 0.0f);
    }
    
    void Player::draw3D(float deltaTime) {
        // Draw a simple triangle for the player
        rdpq_sync_pipe();
        
        rdpq_mode_begin();
        rdpq_mode_zbuf(true, true);
        // Use PRIM color set by rdpq_set_prim_color
        // RDPQ_COMBINER1((PRIM, 0, SHADE, 0), (0, 0, 0, 1)) allows both PRIM color and vertex shading (SHADE) to influence the final color.
        // This might help if there's some basic lighting intended.
        rdpq_mode_combiner(RDPQ_COMBINER1((PRIM, 0, SHADE, 0), (0, 0, 0, 1))); 
        rdpq_mode_blender(0); // No blending for opaque triangle
        rdpq_mode_antialias(AA_NONE);
        rdpq_mode_filter(FILTER_POINT);
        rdpq_mode_dithering(DITHER_NONE_NONE);
        rdpq_mode_persp(true);
        rdpq_mode_fog(0);
        rdpq_mode_end();
        
        // Set color to bright blue (intended player color)
        // Different colors for different players
        if (playerPort == JOYPAD_PORT_1) {
            rdpq_set_prim_color(RGBA32(25, 128, 255, 255)); // Blue for player 1
        } else {
            rdpq_set_prim_color(RGBA32(255, 25, 128, 255)); // Red for player 2
        }
        
        // Draw a triangle - Apply rotation
        float cosR = cosf(rotation);
        float sinR = sinf(rotation);
        
        // Define triangle vertices in local space (centered at origin)
        float x1 = 0, y1 = 5;   // Top vertex
        float x2 = -5, y2 = -5; // Bottom left
        float x3 = 5, y3 = -5;  // Bottom right
        
        // Rotate vertices
        float rx1 = x1 * cosR - y1 * sinR;
        float ry1 = x1 * sinR + y1 * cosR;
        float rx2 = x2 * cosR - y2 * sinR;
        float ry2 = x2 * sinR + y2 * cosR;
        float rx3 = x3 * cosR - y3 * sinR;
        float ry3 = x3 * sinR + y3 * cosR;
        
        // Draw triangle at player's position (on X/Y plane, Z=0)
        rdpq_triangle(&TRIFMT_FILL,
            (float[]){position.x + rx1, position.y + ry1, 0.0f},
            (float[]){position.x + rx2, position.y + ry2, 0.0f},
            (float[]){position.x + rx3, position.y + ry3, 0.0f}
        );
    }
    
    void Player::drawPTX(float deltaTime) {
        // No particle effects for now
    }
}