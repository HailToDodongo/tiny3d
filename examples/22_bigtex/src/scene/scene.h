/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <t3d/t3d.h>
#include "camera.h"
#include "../render/uvTexture.h"
#include "../utils/memory.h"
#include "../render/textures.h"
#include "../render/skybox.h"
#include "../render/hdModel.h"
#include "../render/fbBlend.h"

class Scene
{
  private:
    Textures textures{18};

    Skybox skybox{"rom:/skybox.t3dm", textures};
    HDModel mapModel;
    FbBlend fbBlend{};

    Camera camera{};
    UVTexture uvTex{};

    float lightTimer{0};

  public:
    Scene();

    void update(float deltaTime);
    void draw(const Memory::FrameBuffers &buffers, surface_t *surf);
};