/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once

#include "hdModel.h"
#include "textures.h"
#include "../main.h"

class Skybox
{
  private:
    HDModel skyboxModel;
    Textures &textures;
    int currId{-1};

  public:
    Skybox(const std::string &path, Textures &textures)
     : skyboxModel{path, textures}, textures{textures}
    {}

    void change(int id);

    inline void update(const T3DVec3 &camPos) { skyboxModel.setPos(camPos); }
    inline void draw() { skyboxModel.draw(); }
};