/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneLast64.h"
#include "../../main.h"
#include "../../debugMenu.h"
#include "../../render/debugDraw.h"

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
    
    // Allocate vertices for projectiles (thin quads)
    T3DVec3 normalVec = {{0.0f, 0.0f, 1.0f}};
    uint16_t norm = t3d_vert_pack_normal(&normalVec);
    projVertices = (T3DVertPacked*)malloc_uncached(sizeof(T3DVertPacked) * 20 * 2); // 2 structures per projectile
    for (int i = 0; i < 20; i++) {
        int idx = i * 2;
        // Using a thin quad to represent a line
        // First structure: vertices 0 and 1
        projVertices[idx] = (T3DVertPacked){};
        projVertices[idx].posA[0] = -1; projVertices[idx].posA[1] = -0.5; projVertices[idx].posA[2] = 0;
        projVertices[idx].normA = norm;
        projVertices[idx].posB[0] = 1; projVertices[idx].posB[1] = -0.5; projVertices[idx].posB[2] = 0;
        projVertices[idx].normB = norm;
        // Projectile colors are already bright white, which is good for bloom
        projVertices[idx].rgbaA = 0xFFFFFFFF; // Bright white
        projVertices[idx].rgbaB = 0xFFFFFFFF; // Bright white
        projVertices[idx].stA[0] = 0; projVertices[idx].stA[1] = 0;
        projVertices[idx].stB[0] = 0; projVertices[idx].stB[1] = 0;
        
        // Second structure: vertices 2 and 3 (completing the quad)
        projVertices[idx+1] = (T3DVertPacked){};
        projVertices[idx+1].posA[0] = 1; projVertices[idx+1].posA[1] = 0.5; projVertices[idx+1].posA[2] = 0;
        projVertices[idx+1].normA = norm;
        projVertices[idx+1].posB[0] = -1; projVertices[idx+1].posB[1] = 0.5; projVertices[idx+1].posB[2] = 0;
        projVertices[idx+1].normB = norm;
        projVertices[idx+1].rgbaA = 0xFFFFFFFF; // Bright white
        projVertices[idx+1].rgbaB = 0xFFFFFFFF; // Bright white
        projVertices[idx+1].stA[0] = 0; projVertices[idx+1].stA[1] = 0;
        projVertices[idx+1].stB[0] = 0; projVertices[idx+1].stB[1] = 0;
    }
    
    // Create matrices for projectiles
    for (int i = 0; i < 20; i++) {
        t3d_mat4_identity(&projMat[i]);
        projMatFP[i] = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    }
    
    // Spawn some initial enemies for testing
    for (int i = 0; i < 5; i++) {
        T3DVec3 pos = {{(float)(-10 + i * 5), (float)(10 - i * 5), 0.0f}};
        T3DVec3 vel = {{0.0f, 0.0f, 0.0f}};
        Actor::Enemy::spawn(pos, vel, 10.0f);
    }
}

SceneLast64::~SceneLast64()
{
    delete player; // Clean up player instance
    Actor::Enemy::cleanup(); // Clean up enemy pool
    free_uncached(projVertices);
    for (int i = 0; i < 20; i++) {
        free_uncached(projMatFP[i]);
    }
}

void SceneLast64::updateScene(float deltaTime)
{
    // Update player
    player->update(deltaTime);
    
    // Update all enemies
    Actor::Enemy::updateAll(deltaTime);
    
    // Get player position for enemy positioning
    // In a real game, enemies would move toward the player's actual position
    T3DVec3 playerPos = player->getPosition();
    float playerX = playerPos.x;
    float playerY = playerPos.y;
    
    // Update projectile matrices
    for (int i = 0; i < 5; i++) { // Only update first 5 projectiles to reduce load
        if (projActive[i]) {
            // Move projectiles
            projY[i] += 0.5f;
            
            // Deactivate projectiles that go off screen
            if (projY[i] > 30.0f) {
                projActive[i] = false;
            } else {
                t3d_mat4_identity(&projMat[i]);
                t3d_mat4_translate(&projMat[i], projX[i], projY[i], 0.0f);
                t3d_mat4_to_fixed(projMatFP[i], &projMat[i]);
            }
        }
    }
    
    // Fire projectiles occasionally
    static float fireTimer = 0.0f;
    fireTimer += deltaTime;
    if (fireTimer > 0.5f) { // Slower firing rate
        fireTimer = 0.0f;
        
        // Find an inactive projectile to fire
        for (int i = 0; i < 5; i++) { // Only use first 5 projectiles
            if (!projActive[i]) {
                projActive[i] = true;
                projX[i] = playerX;
                projY[i] = playerY;
                break;
            }
        }
    }
    
    // Spawn new enemies occasionally
    static float enemySpawnTimer = 0.0f;
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer > 1.0f) { // Spawn an enemy every second
        enemySpawnTimer = 0.0f;
        
        // Spawn a new enemy at a random edge of the screen
        float spawnX, spawnY;
        int edge = rand() % 4; // 0=top, 1=right, 2=bottom, 3=left
        
        switch (edge) {
            case 0: // Top
                spawnX = -30 + (rand() % 60);
                spawnY = 30;
                break;
            case 1: // Right
                spawnX = 30;
                spawnY = -30 + (rand() % 60);
                break;
            case 2: // Bottom
                spawnX = -30 + (rand() % 60);
                spawnY = -30;
                break;
            case 3: // Left
                spawnX = -30;
                spawnY = -30 + (rand() % 60);
                break;
            default:
                spawnX = 0;
                spawnY = 0;
                break;
        }
        
        T3DVec3 pos = {{spawnX, spawnY, 0.0f}};
        
        // Calculate velocity toward player
        T3DVec3 playerPos = player->getPosition();
        T3DVec3 direction = {{playerPos.x - spawnX, playerPos.y - spawnY, 0.0f}};
        
        // Normalize direction
        float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
        if (length > 0) {
            direction.x /= length;
            direction.y /= length;
        }
        
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
    
    // Draw projectiles as thin quads
    for (int i = 0; i < 5; i++) { // Only draw first 5 projectiles
        if (projActive[i]) {
            t3d_matrix_push(projMatFP[i]);
            t3d_vert_load(&projVertices[i*2], 0, 4); // Load 4 vertices (2 structures)
            t3d_tri_draw(0, 1, 2);
            t3d_tri_draw(2, 3, 0);
            t3d_tri_sync();
            t3d_matrix_pop(1);
        }
    }
}

void SceneLast64::draw2D(float deltaTime)
{
    // Draw simple 2D HUD elements
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    
    // Draw a simple score indicator
    Debug::printf(200, 20, "LAST64");
    Debug::printf(200, 35, "ENEMIES: %d", Actor::Enemy::getActiveCount());
}