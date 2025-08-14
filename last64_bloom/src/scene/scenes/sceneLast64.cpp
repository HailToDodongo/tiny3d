/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneLast64.h"
#include "../../main.h"
#include "../../debugMenu.h"
#include "../../render/debugDraw.h"
#include <t3d/t3d.h>
#include <t3d/tpx.h>
#include <t3d/t3dmath.h>
#include <libdragon.h>

namespace {
  // Screen boundaries
  constexpr float SCREEN_LEFT = 0.0f;
  constexpr float SCREEN_RIGHT = 312.0f;
  constexpr float SCREEN_TOP = 0.0f;
  constexpr float SCREEN_BOTTOM = 232.0f;
  constexpr float SCREEN_WIDTH = SCREEN_RIGHT - SCREEN_LEFT;
  constexpr float SCREEN_HEIGHT = SCREEN_BOTTOM - SCREEN_TOP;
  
  // Ambient lighting - match SceneMain exactly
  constexpr uint8_t colorAmbient[4] = {0x2A, 0x2A, 0x2A, 0x00};

  // Static matrix for scene
  T3DMat4FP* sceneMatFP = nullptr;
}

SceneLast64::SceneLast64()
{
    // Set up camera - match SceneMain more closely
    camera.fov = T3D_DEG_TO_RAD(80.0f);
    camera.near = 5.0f;
    camera.far = 295.0f;
    camera.pos = {0.0f, 0.0f, 20.0f};
    camera.target = {0.0f, 0.0f, 0.0f};

    // Initialize scene matrix
    sceneMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    t3d_mat4fp_identity(sceneMatFP);

    // Create player instances at different positions
    // All actors exist in 3D space now, with Z=0 for the playing field
    T3DVec3 startPos1 = {{0.0f, 0.0f, 0.0f}};  // Center of screen
    T3DVec3 startPos2 = {{20.0f, 0.0f, 0.0f}};  // Slightly to the right
    player1 = new Actor::Player(startPos1, JOYPAD_PORT_1);
    player2 = new Actor::Player(startPos2, JOYPAD_PORT_2);
    
    // Initialize enemy pool
    Actor::Enemy::initialize();
}

SceneLast64::~SceneLast64()
{
    delete player1; // Clean up player1 instance
    delete player2; // Clean up player2 instance
    Actor::Enemy::cleanup(); // Clean up enemy pool
    
    // Clean up scene matrix
    if (sceneMatFP) {
        free_uncached(sceneMatFP);
        sceneMatFP = nullptr;
    }
}

void SceneLast64::updateScene(float deltaTime)
{
    // Update camera
    camera.update(deltaTime);
    camera.attach();
    
    // Update players (this will also update their weapons)
    player1->update(deltaTime);
    player2->update(deltaTime);
    
    // Update all enemies
    Actor::Enemy::updateAll(deltaTime);
    
    // Update all projectiles (this is now handled by the player's weapon)
    Actor::Projectile::updateAll(deltaTime);
    
    // Get player positions for enemy positioning
    T3DVec3 player1Pos = player1->getPosition();
    T3DVec3 player2Pos = player2->getPosition();
    
    // Spawn new enemies occasionally
    static float enemySpawnTimer = 0.0f;
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer > 0.3f) { // Spawn an enemy every 0.3 seconds
        enemySpawnTimer = 0.0f;
        
        // Spawn a new enemy at a random edge of the screen
        float spawnX, spawnY;
        int edge = rand() % 4; // 0=top, 1=right, 2=bottom, 3=left
        
        switch (edge) {
            case 0: // Top
                spawnX = SCREEN_LEFT + (rand() % (int)SCREEN_WIDTH);
                spawnY = SCREEN_TOP;
                break;
            case 1: // Right
                spawnX = SCREEN_RIGHT;
                spawnY = SCREEN_TOP + (rand() % (int)SCREEN_HEIGHT);
                break;
            case 2: // Bottom
                spawnX = SCREEN_LEFT + (rand() % (int)SCREEN_WIDTH);
                spawnY = SCREEN_BOTTOM;
                break;
            case 3: // Left
                spawnX = SCREEN_LEFT;
                spawnY = SCREEN_TOP + (rand() % (int)SCREEN_HEIGHT);
                break;
            default:
                spawnX = 0;
                spawnY = 0;
                break;
        }
        
        T3DVec3 pos = {{spawnX, spawnY, 0.0f}};
        
        // Spawn enemy with zero initial velocity (will be calculated by enemy itself)
        // All actors exist in the same 3D space with Z=0 for the playing field
        Actor::Enemy::spawn(pos, 45.0f);
    }
}

void SceneLast64::draw3D(float deltaTime)
{
    // Attach camera
    camera.attach();
    
    // Clear screen with a dark grey color (similar to original but darker)
    t3d_screen_clear_color(RGBA32(32, 32, 32, 0xFF)); // Dark grey background
    // Clear depth buffer
    t3d_screen_clear_depth();
    
    // Set up environment color for bloom effect
    rdpq_set_env_color({0xFF, 0xAA, 0xEE, 0xAA});

    // Set up lighting to match SceneMain
    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(0); // No directional lights, just ambient

    // Push scene matrix
    t3d_matrix_push(sceneMatFP);

    // Set up rendering state
    t3d_state_set_drawflags((enum T3DDrawFlags)(T3D_FLAG_SHADED | T3D_FLAG_DEPTH));

    // Draw players using the Player class (this will also draw their weapons)
    player1->draw3D(deltaTime);
    player2->draw3D(deltaTime);
    
    // Draw all enemies
    Actor::Enemy::drawAll(deltaTime);
    
    // Draw all projectiles (this is now handled by the player's weapon)
    Actor::Projectile::drawAll(deltaTime);

    // Pop scene matrix
    t3d_matrix_pop(1);
}

void SceneLast64::draw2D(float deltaTime)
{   
    // Draw player positions
    if (player1) {
        T3DVec3 playerPos = player1->getPosition();
        Debug::printf(10, 10, "P1 x:%.0f y:%.0f", playerPos.x, playerPos.y);
    }
    
    if (player2) {
        T3DVec3 playerPos = player2->getPosition();
        Debug::printf(10, 20, "P2 x:%.0f y:%.0f", playerPos.x, playerPos.y);
    }
    
    // Draw enemy and projectile counts
    Debug::printf(100, 5, "E:%d", Actor::Enemy::getActiveCount());
    Debug::printf(200, 5, "P:%d", Actor::Projectile::getActiveCount());
}