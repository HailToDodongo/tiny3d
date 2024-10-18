/**
* @copyright 2023 - Max Bebök
* @license MIT
*/

#define CGLTF_IMPLEMENTATION

#include "parser.h"
#include "hash.h"

#include "math/vec2.h"
#include "math/vec3.h"

#include "lib/meshopt/meshoptimizer.h"
#include "math/mat4.h"
#include "parser/parser.h"
#include "converter/converter.h"

void printBoneTree(const Bone &bone, int depth)
{
  for(int i=0; i<depth; ++i)printf("  ");
  printf("%s\n", bone.name.c_str());
  bone.parentMatrix.print(depth * 2);
  for(auto &child : bone.children) {
    printBoneTree(*child, depth+1);
  }
}

T3DMData parseGLTF(const char *gltfPath, float modelScale)
{
  T3DMData t3dm{};
  fs::path gltfBasePath{gltfPath};
  gltfBasePath = gltfBasePath.parent_path();

  cgltf_options options{};
  cgltf_data* data = nullptr;
  cgltf_result result = cgltf_parse_file(&options, gltfPath, &data);

  if(result == cgltf_result_file_not_found) {
    fprintf(stderr, "Error: File not found! (%s)\n", gltfPath);
    throw std::runtime_error("File not found!");
  }

  if(cgltf_validate(data) != cgltf_result_success) {
    fprintf(stderr, "Invalid glTF data!\n");
    throw std::runtime_error("Invalid glTF data!");
  }

  cgltf_load_buffers(&options, data, gltfPath);

  // Bones / Armature
  int boneCount = 0;
  int neutralBoneCount = 0;
  for(int i=0; i<data->skins_count; ++i) {
    auto &skin = data->skins[i];
    //printf(" - Skeleton: %s\n", skin.skeleton->name);
    if(skin.joints_count == 0)continue;

    while((boneCount+neutralBoneCount) < skin.joints_count)
    {
      const cgltf_node* bone = skin.joints[boneCount];

      // vertices not assigned to any bone are assigned to an artificial "neutral_bone"
      if(strcmp(bone->name, "neutral_bone") == 0) {
        neutralBoneCount++;
        continue;
      }

      Bone armature = parseBoneTree(bone, nullptr, boneCount);

      //printBoneTree(armature, 0);
      t3dm.skeletons.push_back(armature);
    }
  }

  // Resting pose matrix stack, used to pre-transform vertices
  std::vector<Mat4> matrixStack{};
  std::unordered_map<std::string, const Bone*> boneMap{};
  if(!t3dm.skeletons.empty()) {
    auto addBoneMax = [&](auto&& addBoneMax, const Bone &bone) -> void {
      matrixStack.push_back(bone.inverseBindPose);
      boneMap[bone.name] = &bone;
      for(auto &child : bone.children) {
        addBoneMax(addBoneMax, *child);
      }
    };
    for(const auto &skel : t3dm.skeletons) {
      addBoneMax(addBoneMax, skel);
    }
  }

  // Animations
  //printf("Animations: %d\n", data->animations_count);

  for(int i=0; i<data->animations_count; ++i) {
    auto anim = parseAnimation(data->animations[i], boneMap, config.animSampleRate);
    if(anim.duration < 0.0001f)continue; // ignore empty animations
    convertAnimation(anim, boneMap);
    t3dm.animations.push_back(anim);
  }

  // Meshes
  for(int i=0; i<data->nodes_count; ++i)
  {
    auto node = &data->nodes[i];
    //printf("- Node %d: %s\n", i, node->name);

    auto mesh = node->mesh;
    if(!mesh)continue;

    // printf(" - Mesh %d: %s\n", i, mesh->name);

    for(int j = 0; j < mesh->primitives_count; j++)
    {
      t3dm.models.push_back({});
      auto &model = t3dm.models.back();
      if(node->name)model.name = node->name;

      auto prim = &mesh->primitives[j];
      //printf("   - Primitive %d:\n", j);

      if(prim->material) {
        parseMaterial(gltfBasePath, i, j, model, prim);
      }

      // find vertex count
      int vertexCount = 0;
      for(int k = 0; k < prim->attributes_count; k++) {
        if(prim->attributes[k].type == cgltf_attribute_type_position) {
          vertexCount = prim->attributes[k].data->count;
          break;
        }
      }

      std::vector<VertexNorm> vertices{};
      vertices.resize(vertexCount, {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .boneIndex = -1});
      std::vector<uint16_t> indices{};

      // Read indices
      if(prim->indices != nullptr)
      {
        auto acc = prim->indices;
        auto basePtr = ((uint8_t*)acc->buffer_view->buffer->data) + acc->buffer_view->offset + acc->offset;
        auto elemSize = Gltf::getDataSize(acc->component_type);

        for(int k = 0; k < acc->count; k++) {
          indices.push_back(Gltf::readAsU32(basePtr, acc->component_type));
          basePtr += elemSize;
        }
      }

      // Read vertices
      for(int k = 0; k < prim->attributes_count; k++)
      {
        auto attr = &prim->attributes[k];
        auto acc = attr->data;
        auto basePtr = ((uint8_t*)acc->buffer_view->buffer->data) + acc->buffer_view->offset + acc->offset;
        auto elemSize = Gltf::getDataSize(acc->component_type);

        // printf("     - Attribute %d: %s\n", k, attr->name);
        if(attr->type == cgltf_attribute_type_position)
        {
          assert(attr->data->type == cgltf_type_vec3);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.pos[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.pos[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.pos[2] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
          }
        }

        if(attr->type == cgltf_attribute_type_color)
        {
          assert(attr->data->type == cgltf_type_vec4);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.color[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.color[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.color[2] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.color[3] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;

            // linear to gamma
            for(int c=0; c<3; ++c) {
              v.color[c] = powf(v.color[c], 0.4545f);
            }
          }
        }

        if(attr->type == cgltf_attribute_type_normal)
        {
          assert(attr->data->type == cgltf_type_vec3);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.norm[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.norm[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.norm[2] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
          }
        }

        if(attr->type == cgltf_attribute_type_texcoord)
        {
          assert(attr->data->type == cgltf_type_vec2);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.uv[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.uv[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
          }
        }

        if(attr->type == cgltf_attribute_type_joints)
        {
          assert(attr->data->type == cgltf_type_vec4);
          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            u32 joins[4];
            for(int c=0; c<4; ++c) {
              joins[c] = Gltf::readAsU32(basePtr, acc->component_type); basePtr += elemSize;
            }
            //printf("  - %d %d %d %d\n", joins[0], joins[1], joins[2], joins[3]);
            v.boneIndex = joins[0];
            if(v.boneIndex >= boneCount || v.boneIndex < 0)v.boneIndex = -1;
          }
        }

        if(attr->type == cgltf_attribute_type_weights)
        {
          assert(attr->data->type == cgltf_type_vec4);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            float weights[4];
            for(int c=0; c<4; ++c) {
              weights[c] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            }
            //printf("  - %f %f %f %f\n", weights[0], weights[1], weights[2], weights[3]);
          }
        }
      }

      std::vector<VertexT3D> verticesT3D{};
      verticesT3D.resize(vertices.size());

      float texSizeX = model.material.texA.texWidth;
      float texSizeY = model.material.texA.texHeight;

      if(texSizeX == 0)texSizeX = 32;
      if(texSizeY == 0)texSizeY = 32;

      // convert vertices
      for(int k = 0; k < vertices.size(); k++) {
        Mat4 mat = parseNodeMatrix(node);
        convertVertex(
          modelScale, texSizeX, texSizeY, vertices[k], verticesT3D[k],
          mat, matrixStack
        );
      }

      // optimizations
      meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), vertices.size());
      //meshopt_optimizeOverdraw(indices.data(), indices.data(), indices.size(), &vertices[0].pos.data[0], vertices.size(), sizeof(VertexNorm), 1.05f);

      // expand into triangles, this is used to split up and dedupe data
      model.triangles.reserve(indices.size() / 3);

      for(int k = 0; k < indices.size(); k += 3) {
        model.triangles.push_back({
          verticesT3D[indices[k + 0]],
          verticesT3D[indices[k + 1]],
          verticesT3D[indices[k + 2]],
        });
      }
    }
  }

  cgltf_free(data);
  return t3dm;
}
