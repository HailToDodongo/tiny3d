/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once

#include "../math/mat4.h"
#include "../structs.h"

void convertVertex(
  float modelScale, float texSizeX, float texSizeY, const VertexNorm &v, VertexT3D &vT3D,
  const Mat4 &mat, const std::vector<Mat4> &matrices, bool uvAdjust
);
ModelChunked chunkUpModel(const Model& model);

void convertAnimation(Anim &anim, const std::unordered_map<std::string, const Bone*> &nodeMap);