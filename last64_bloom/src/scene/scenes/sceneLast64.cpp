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

    // Create player instance at the center of the screen
    T3DVec3 startPos = {{80.0f, 50.0f, 0.0f}};
    player = new Actor::Player(startPos);
    
    // Initialize enemy pool
    Actor::Enemy::initialize();
    
    // Create weapon instance
    weapon = new Actor::ProjectileWeapon();
    
    // Set player reference in weapon
    Actor::Weapon::setPlayer(player);
}

SceneLast64::~SceneLast64()
{
    delete player; // Clean up player instance
    delete weapon; // Clean up weapon instance
    Actor::Enemy::cleanup(); // Clean up enemy pool
}

void SceneLast64::updateScene(float deltaTime)
{
    // Update player
    player->update(deltaTime);
    
    // Update weapon
    weapon->update(deltaTime);
    
    // Update all enemies
    Actor::Enemy::updateAll(deltaTime);
    
    // Update all projectiles
    Actor::Projectile::updateAll(deltaTime);
    
    // Get player position for enemy positioning
    // In a real game, enemies would move toward the player's actual position
    T3DVec3 playerPos = player->getPosition();
    
    // Spawn new enemies occasionally
    static float enemySpawnTimer = 0.0f;
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer > 0.3f) { // Spawn an enemy every second
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
        T3DVec3 direction = {{0.0f, 0.0f, 0.0f}};
        Actor::Enemy::spawn(pos, direction, 10.0f);
    }
}

void SceneLast64::draw3D(float deltaTime)
{
    // Clear screen with a very bright color to see triangles clearly
    t3d_screen_clear_color(RGBA32(255, 255, 255, 0xFF)); // Pure white background
    // Clear depth buffer
    t3d_screen_clear_depth();

    // Simple lighting - Increase ambient light significantly for better visibility
    // Consider adding a directional light from above if needed
    uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0xFF}; // Full white ambient
    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(1); // Use 1 light (ambient only)
    
    // Set draw flags properly - Ensure lighting and other features are enabled correctly
    // T3D_FLAG_SHADED is crucial for using vertex colors/lights
    // T3D_FLAG_DEPTH for depth testing
    // Other flags might be needed depending on desired effects (e.g., T3D_FLAG_TEXTURE for textures, though not used here)
    t3d_state_set_drawflags((enum T3DDrawFlags)(T3D_FLAG_SHADED | T3D_FLAG_DEPTH));

    // Draw player using the Player class
    player->draw3D(deltaTime);
    
    // Draw all enemies
    Actor::Enemy::drawAll(deltaTime);
    
    // Draw all projectiles via weapon
    weapon->draw3D(deltaTime);
}

void SceneLast64::draw2D(float deltaTime)
{   
    // Draw player position
    if (player) {
        T3DVec3 playerPos = player->getPosition();
        Debug::printf(10, 10, "PLAYER POS: %.0f, %.0f", playerPos.x, playerPos.y);
    }
    
    // Draw enemy and projectile counts
    Debug::printf(200, 35, "ENEMIES: %d", Actor::Enemy::getActiveCount());
    Debug::printf(200, 50, "PROJECTILES: %d", Actor::Projectile::getActiveCount());
}