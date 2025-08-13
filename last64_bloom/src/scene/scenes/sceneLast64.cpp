/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneLast64.h"
#include "../../main.h"
#include "../../debugMenu.h"
#include "../../render/debugDraw.h"

namespace {
  // Screen boundaries
  constexpr float SCREEN_LEFT = 0.0f;
  constexpr float SCREEN_RIGHT = 312.0f;
  constexpr float SCREEN_TOP = 0.0f;
  constexpr float SCREEN_BOTTOM = 236.0f;
  constexpr float SCREEN_WIDTH = SCREEN_RIGHT - SCREEN_LEFT;
  constexpr float SCREEN_HEIGHT = SCREEN_BOTTOM - SCREEN_TOP;
}

SceneLast64::SceneLast64()
{
    // Set up camera
    camera.fov = T3D_DEG_TO_RAD(85.0f);
    camera.near = 1.0f;
    camera.far = 100.0f;
    camera.pos = {0.0f, 0.0f, 20.0f};
    camera.target = {0.0f, 0.0f, 0.0f};

    // Create player instances at different positions
    T3DVec3 startPos1 = {{80.0f, 50.0f, 0.0f}};
    T3DVec3 startPos2 = {{120.0f, 50.0f, 0.0f}};
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
}

void SceneLast64::updateScene(float deltaTime)
{
    // Update players
    player1->update(deltaTime);
    player2->update(deltaTime);
    
    // Update all enemies
    Actor::Enemy::updateAll(deltaTime);
    
    // Update all projectiles
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
        Actor::Enemy::spawn(pos, 45.0f);
    }
}

void SceneLast64::draw3D(float deltaTime)
{
    // Clear screen with a grey color
    t3d_screen_clear_color(RGBA32(128, 128, 128, 0xFF)); // Grey background
    // Clear depth buffer
    t3d_screen_clear_depth();

    // Simple lighting
    uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0xFF}; // Full white ambient
    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(1); // Use 1 light (ambient only)
    
    // Set draw flags properly
    t3d_state_set_drawflags((enum T3DDrawFlags)(T3D_FLAG_SHADED | T3D_FLAG_DEPTH));

    // Draw players using the Player class
    player1->draw3D(deltaTime);
    player2->draw3D(deltaTime);
    
    // Draw all enemies
    Actor::Enemy::drawAll(deltaTime);
    
    // Draw all projectiles
    Actor::Projectile::drawAll(deltaTime);
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