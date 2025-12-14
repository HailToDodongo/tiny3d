/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once
#include "../structs.h"

namespace T3DM
{
  void optimizeModelChunk(ModelChunked &model);
  std::vector<int16_t> createMeshBVH(const std::vector<ModelChunked> &modelChunks);
}