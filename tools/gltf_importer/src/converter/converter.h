/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once

#include "../math/mat4.h"
#include "../structs.h"

void convertVertex(
  float modelScale, float texSizeX, float texSizeY, const T3DM::VertexNorm &v, T3DM::VertexT3D &vT3D,
  const Mat4 &mat, const std::vector<Mat4> &matrices, bool uvAdjust
);
T3DM::ModelChunked chunkUpModel(const T3DM::Model& model);

void convertAnimation(T3DM::Anim &anim, const std::unordered_map<std::string, const T3DM::Bone*> &nodeMap);