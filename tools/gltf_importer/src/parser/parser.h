/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

#include "../math/mat4.h"
#include "../cgltfHelper.h"
#include "../structs.h"

namespace fs = std::filesystem;

void parseMaterial(const fs::path &gltfBasePath, int i, int j, Model &model, cgltf_primitive *prim);
Mat4 parseNodeMatrix(const cgltf_node *node, const Vec3 &posScale = {1.0f, 1.0f, 1.0f});
Bone parseBoneTree(const cgltf_node *rootBone, Bone *parentBone, int &count);