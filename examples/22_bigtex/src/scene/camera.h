/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <t3d/t3d.h>

class Camera
{
  private:
    T3DViewport viewport{};

    fm_vec3_t pos{{0,0,0}};
    fm_vec3_t target{{0,0,0}};
    fm_vec3_t dir{{1,0,0}};

    fm_vec3_t targetPos{pos};

    float rotX{0};
    float rotY{0};
    float camLerpSpeed{0};

    float targetRotX{rotX};
    float targetRotY{rotY};

  public:
    Camera();

    void update(float deltaTime);
    void attach();
    void setCamLerpSpeed(float speed) { camLerpSpeed = speed; }

    const fm_vec3_t& getPosition() const { return pos; }
};