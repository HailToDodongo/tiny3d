/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <t3d/t3d.h>

class Camera
{
  private:
    T3DViewport viewport[3]{};

    T3DVec3 pos{{0,0,0}};
    T3DVec3 target{{0,0,0}};
    T3DVec3 dir{{1,0,0}};

    T3DVec3 targetPos{pos};

    float rotX{0};
    float rotY{0};
    float camLerpSpeed{0};

    float targetRotX{rotX};
    float targetRotY{rotY};

    int frameIdx{0};

  public:
    Camera();

    void update(float deltaTime);
    void attach();
    void attachLast();
    void setCamLerpSpeed(float speed) { camLerpSpeed = speed; }

    const T3DVec3& getPosition() const { return pos; }
    const T3DVec3& getTarget() const { return target; }
};