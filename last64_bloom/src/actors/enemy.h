/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../actors/base.h"
#include "player.h"
#include <t3d/t3d.h>

#define MAX_ENEMIES 100

namespace Actor {
    class Enemy : public Base {
    private:
        static T3DVertPacked* sharedVertices;
        static T3DMat4FP** sharedMatrices;
        static bool* activeFlags;
        static uint32_t activeCount;
        static bool initialized;
        static Enemy enemyPool[MAX_ENEMIES]; // Static pool of enemies

        T3DVec3 position;
        float speed;
        uint32_t poolIndex;
        Player* targetPlayer; // Individual target player for this enemy

        static void initializePool();

    public:
        Enemy();
        ~Enemy();

        static void initialize();
        static void cleanup();
        static Enemy* spawn(const T3DVec3& position, float speed, Player* player1, Player* player2);
        static void updateAll(float deltaTime);
        static void drawAll(float deltaTime);
        static uint32_t getActiveCount() { return activeCount; }
        
        // Method to set the global target player (kept for compatibility)
        // static void setTargetPlayer(Player* player) { globalTargetPlayer = player; }

        void update(float deltaTime) override;
        void draw3D(float deltaTime) override;
        void drawPTX(float deltaTime) override;

        void deactivate();
        bool isActive() const;

        T3DVec3 getPosition() const { return position; }
        void setPosition(const T3DVec3& newPos) { position = newPos; }
        
        // Method to set individual target player for this enemy
        void setIndividualTargetPlayer(Player* player) { targetPlayer = player; }
    };
}