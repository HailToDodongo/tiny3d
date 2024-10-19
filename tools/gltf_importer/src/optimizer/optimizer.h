/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once
#include "../structs.h"

void optimizeModelChunk(ModelChunked &model);
std::vector<int16_t> createMeshBVH(const std::vector<ModelChunked> &modelChunks);