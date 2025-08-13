/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "../actors/base.h"
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

        static void initializePool();

    public:
        Enemy();
        ~Enemy();

        static void initialize();
        static void cleanup();
        static Enemy* spawn(const T3DVec3& position, float speed);
        static void updateAll(float deltaTime);
        static void drawAll(float deltaTime);
        static uint32_t getActiveCount() { return activeCount; }

        void update(float deltaTime) override;
        void draw3D(float deltaTime) override;
        void drawPTX(float deltaTime) override;

        void deactivate();
        bool isActive() const;

        T3DVec3 getPosition() const { return position; }
        void setPosition(const T3DVec3& newPos) { position = newPos; }
    };
}