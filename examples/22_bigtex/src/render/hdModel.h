/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include <string>
#include <t3d/t3dmodel.h>
#include "textures.h"
#include "ringMat.h"

class HDModel
{
  private:
    T3DModel *model{};
    RingMat4FP matFP{};
    rspq_block_t *dplDraw{};
    rspq_block_t *dplDrawShade{};

  public:
    HDModel(const std::string &t3dmPath, Textures& textures);
    ~HDModel();

    void setPos(const T3DVec3 &pos);
    void draw();
    void drawShade();
};