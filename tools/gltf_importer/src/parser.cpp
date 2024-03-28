/**
* @copyright 2023 - Max Bebök
* @license MIT
*/

#define CGLTF_IMPLEMENTATION
#include "cgltfHelper.h"

#include "lib/json.hpp"
using json = nlohmann::json;

#include "lib/lodepng.h"
#include "parser.h"
#include "fast64Types.h"
#include "hash.h"

#include "math/vec2.h"
#include "math/vec3.h"

#include "lib/meshopt/meshoptimizer.h"
#include "math/mat4.h"

namespace fs = std::filesystem;

namespace {
  constexpr uint64_t RDPQ_COMBINER_2PASS = (uint64_t)(1) << 63;

  #define rdpq_1cyc_comb_rgb(suba, subb, mul, add) \
    (((uint64_t)(suba)<<52) | ((uint64_t)(subb)<<28) | ((uint64_t)(mul)<<47) | ((uint64_t)(add)<<15) | \
     ((uint64_t)(suba)<<37) | ((uint64_t)(subb)<<24) | ((uint64_t)(mul)<<32) | ((uint64_t)(add)<<6))
  #define rdpq_1cyc_comb_alpha(suba, subb, mul, add) \
    (((uint64_t)(suba)<<44) | ((uint64_t)(subb)<<12) | ((uint64_t)(mul)<<41) | ((uint64_t)(add)<<9) | \
     ((uint64_t)(suba)<<21) | ((uint64_t)(subb)<<3)  | ((uint64_t)(mul)<<18) | ((uint64_t)(add)<<0))

  #define rdpq_2cyc_comb2a_rgb(suba, subb, mul, add)   ((((uint64_t)suba)<<52) | (((uint64_t)subb)<<28) | (((uint64_t)mul)<<47) | (((uint64_t)add)<<15))
  #define rdpq_2cyc_comb2a_alpha(suba, subb, mul, add) ((((uint64_t)suba)<<44) | (((uint64_t)subb)<<12) | (((uint64_t)mul)<<41) | (((uint64_t)add)<<9))
  #define rdpq_2cyc_comb2b_rgb(suba, subb, mul, add)   ((((uint64_t)suba)<<37) | (((uint64_t)subb)<<24) | (((uint64_t)mul)<<32) | (((uint64_t)add)<<6))
  #define rdpq_2cyc_comb2b_alpha(suba, subb, mul, add) ((((uint64_t)suba)<<21) | (((uint64_t)subb)<<3)  | (((uint64_t)mul)<<18) | (((uint64_t)add)<<0))

  void readMaterialTileAxisFromJson(TileParam &param, const json &tex)
  {
    if(tex.empty())return;
    param.clamp = tex.value<uint8_t>("clamp", 0);
    param.high = tex["high"].get<float>();
    param.low = tex["low"].get<float>();
    param.mask = tex["mask"].get<int8_t>();
    param.mirror = tex.value<int8_t>("mirror", 0);
    param.shift = tex["shift"].get<int8_t>();
  }

  void readMaterialFromJson(Material &material, const json &tex, const fs::path &gltfPath)
  {
    if(tex.contains("S"))readMaterialTileAxisFromJson(material.s, tex["S"]);
    if(tex.contains("T"))readMaterialTileAxisFromJson(material.t, tex["T"]);

    // default texture size, can be overwritten by a texture or reference later
    material.texWidth = material.s.high - material.s.low + 1;
    material.texHeight = material.t.high - material.t.low + 1;

    bool isRef = false;
    if(tex.contains("use_tex_reference")) {
      isRef = tex["use_tex_reference"].get<uint32_t>() == 1;
    }

    // a texture can either be an image loaded from a sprite,
    // or a "reference" which doesn't actively load an image itself
    if(isRef) {
      if(tex.contains("tex_reference")) {
        std::string refAddress = tex["tex_reference"].get<std::string>();

        // try to parse it as a hex string or decimal
        int base = (refAddress.size() > 2 && refAddress[0] == '0' && refAddress[1] == 'x') ? 16 : 10;
        material.texReference = std::stoul(refAddress, nullptr, base);
      }

      if(tex.contains("tex_reference_size")) {
        material.texWidth = tex["tex_reference_size"][0].get<uint32_t>();
        material.texHeight = tex["tex_reference_size"][1].get<uint32_t>();
      }
    }
    else if(tex.contains("tex") && tex["tex"].contains("name"))
    {
      material.texPath = tex["tex"]["name"].get<std::string>();
      if(material.texPath[0] != '/') {
        material.texPath = (gltfPath / fs::path(material.texPath)).string();

        std::vector<unsigned char> image; // pixels
        auto error = lodepng::decode(image, material.texWidth, material.texHeight, material.texPath);
        if(error) {
          printf("Error loading texture %s: %s\n", material.texPath.c_str(), lodepng_error_text(error));
        }
      }
      printf("Loaded Texture %s, size: %dx%d\n", material.texPath.c_str(), material.texWidth, material.texHeight);
    }
  }

  ColorCombiner readCCFromJson(const json &cc)
  {
    ColorCombiner res{};
    res.a = cc["A"].get<uint8_t>();
    res.b = cc["B"].get<uint8_t>();
    res.c = cc["C"].get<uint8_t>();
    res.d = cc["D"].get<uint8_t>();
    res.aAlpha = cc["A_alpha"].get<uint8_t>();
    res.bAlpha = cc["B_alpha"].get<uint8_t>();
    res.cAlpha = cc["C_alpha"].get<uint8_t>();
    res.dAlpha = cc["D_alpha"].get<uint8_t>();

    //printf("Color: (%d - %d) * %d + %d\n", res.a, res.b, res.c, res.d);
    //printf("Alpha: (%d - %d) * %d + %d\n", res.aAlpha, res.bAlpha, res.cAlpha, res.dAlpha);

    return res;
  }

  bool isCCUsingTexture(const ColorCombiner &cc)
  {
    if(cc.a == CC::TEX0 || cc.b == CC::TEX0 || cc.c == CC::TEX0 || cc.d == CC::TEX0)return true;
    if(cc.a == CC::TEX1 || cc.b == CC::TEX1 || cc.c == CC::TEX1 || cc.d == CC::TEX1)return true;

    if(cc.c == CC::TEX0_ALPHA || cc.c == CC::TEX1_ALPHA)return true;

    if(cc.aAlpha == CC::TEX0 || cc.bAlpha == CC::TEX0 || cc.cAlpha == CC::TEX0 || cc.dAlpha == CC::TEX0)return true;
    if(cc.aAlpha == CC::TEX1 || cc.bAlpha == CC::TEX1 || cc.cAlpha == CC::TEX1 || cc.dAlpha == CC::TEX1)return true;

    return false;
  }

  bool isUsingShade(const ColorCombiner &cc)
  {
    if(cc.a == CC::SHADE || cc.b == CC::SHADE || cc.c == CC::SHADE || cc.d == CC::SHADE)return true;
    if(cc.aAlpha == CC::SHADE || cc.bAlpha == CC::SHADE || cc.cAlpha == CC::SHADE || cc.dAlpha == CC::SHADE)return true;
    return false;
  }
}

std::vector<Model> parseGLTF(const char *gltfPath, float modelScale)
{
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
  std::vector<Model> models{};

  printf("Node count: %d\n", data->nodes_count);
  for(int i=0; i<data->nodes_count; ++i)
  {
    auto node = &data->nodes[i];
    auto mesh = node->mesh;
    if(!mesh)continue;

    printf(" - Mesh %d: %s\n", i, mesh->name);

    for(int j = 0; j < mesh->primitives_count; j++)
    {
      models.push_back({});
      auto &model = models.back();

      auto prim = &mesh->primitives[j];
      printf("   - Primitive %d:\n", j);

      if(prim->material) {

        model.materialA.uuid = j*1000+i;
        if(prim->material->name) {
          model.materialA.uuid = stringHash(prim->material->name);
        }
        printf("     Material: %s\n", prim->material->name);

        if(prim->material->extras.data == nullptr) {
          throw std::runtime_error(
            "\n\n"
            "Material has no fast64 data! (@TODO: implement fallback)\n"
            "If you are using fast64, make sure to enable 'Include -> Custom Properties' during GLTF export\n"
            "\n\n"
          );
        }

        auto data = json::parse(prim->material->extras.data);
        if(!prim->material->extras.data) {
          printf("Empty fast64 extras.data!\n");
        }
        auto &f3dData = data["f3d_mat"];

        if(!f3dData.empty()) {
          //printf("  - %s\n", f3dData.dump(2).c_str());
          //printf("  - %s\n", f3dData["combiner2"].dump(2).c_str());

          auto cc1 = readCCFromJson(f3dData["combiner1"]);
          auto cc2 = readCCFromJson(f3dData["combiner2"]);
          bool is2Cycle = true;

          model.materialA.drawFlags = DrawFlags::DEPTH;

          if(f3dData.contains("rdp_settings"))
          {
            auto &rdpSettings = f3dData["rdp_settings"];
            is2Cycle = rdpSettings["g_mdsft_cycletype"].get<uint32_t>() != 0;

            if(rdpSettings["g_cull_back"].get<uint32_t>() != 0) {
              model.materialA.drawFlags |= DrawFlags::CULL_BACK;
            }
            if(rdpSettings["g_cull_front"].get<uint32_t>() != 0) {
              model.materialA.drawFlags |= DrawFlags::CULL_FRONT;
            }

            model.materialA.fogMode = rdpSettings["g_fog"].get<uint32_t>() + 1;
            model.materialB.fogMode = model.materialA.fogMode;

            bool setRenderMode = rdpSettings["set_rendermode"].get<uint32_t>() != 0;
            if(setRenderMode) {
              int renderMode1Raw = rdpSettings["rendermode_preset_cycle_1"].get<uint32_t>();
              int renderMode2Raw = rdpSettings["rendermode_preset_cycle_2"].get<uint32_t>();
              uint8_t alphaMode1 = F64_RENDER_MODE_1_TO_ALPHA[renderMode1Raw];
              uint8_t alphaMode2 = F64_RENDER_MODE_2_TO_ALPHA[renderMode2Raw];

              if(alphaMode1 == AlphaMode::INVALID || alphaMode2 == AlphaMode::INVALID) {
                printf("\n\nInvalid render-modes: %d, please only use Opaque, Cutout, Transparent, Fog-Shade\n", renderMode1Raw);
                throw std::runtime_error("Invalid render-modes!");
              }

              model.materialA.alphaMode = is2Cycle ? alphaMode2 : alphaMode1;
              model.materialB.alphaMode = alphaMode2;
            }
          }

          if(isUsingShade(cc1) || (is2Cycle && isUsingShade(cc2))) {
            model.materialA.drawFlags |= DrawFlags::SHADED;
          }

          if(isCCUsingTexture(cc1) || (is2Cycle && isCCUsingTexture(cc2))) {
            model.materialA.drawFlags |= DrawFlags::TEXTURED;

            if(f3dData.contains("tex0"))readMaterialFromJson(model.materialA, f3dData["tex0"], gltfBasePath);
            if(f3dData.contains("tex1"))readMaterialFromJson(model.materialB, f3dData["tex1"], gltfBasePath);
          }

          if(is2Cycle) {
            model.materialA.colorCombiner  = RDPQ_COMBINER_2PASS |
              rdpq_2cyc_comb2a_rgb(cc1.a, cc1.b, cc1.c, cc1.d) |
              rdpq_2cyc_comb2a_alpha(cc1.aAlpha, cc1.bAlpha, cc1.cAlpha, cc1.dAlpha) |
              rdpq_2cyc_comb2b_rgb(cc2.a, cc2.b, cc2.c, cc2.d) |
              rdpq_2cyc_comb2b_alpha(cc2.aAlpha, cc2.bAlpha, cc2.cAlpha, cc2.dAlpha);
          } else {
            model.materialA.colorCombiner  =
              rdpq_1cyc_comb_rgb(cc1.a, cc1.b, cc1.c, cc1.d) |
              rdpq_1cyc_comb_alpha(cc1.aAlpha, cc1.bAlpha, cc1.cAlpha, cc1.dAlpha);
          }

        } else {
          printf("No Fast64 Material data found!\n");
        }
        model.materialB.colorCombiner = model.materialA.colorCombiner;
        model.materialB.drawFlags = model.materialA.drawFlags;
        model.materialB.uuid = model.materialA.uuid ^ 0x12345678;
      }

      // find vertex count
      int vertexCount = 0;
      for(int k = 0; k < prim->attributes_count; k++) {
        if(prim->attributes[k].type == cgltf_attribute_type_position) {
          vertexCount = prim->attributes[k].data->count;
          break;
        }
      }

      printf("Vertex Input Count: %d\n", vertexCount);

      std::vector<VertexNorm> vertices{};
      vertices.resize(vertexCount, {.color = {1.0f, 1.0f, 1.0f, 1.0f}});
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

        printf("     - Attribute %d: %s\n", k, attr->name);
        if(attr->type == cgltf_attribute_type_position)
        {
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
          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.uv[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.uv[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
          }
        }
      }

      std::vector<VertexT3D> verticesT3D{};
      verticesT3D.resize(vertices.size());
      printf("Vertices: %d\n", vertices.size());

      float texSizeX = model.materialA.texWidth;
      float texSizeY = model.materialA.texHeight;

      // convert vertices
      for(int k = 0; k < vertices.size(); k++)
      {
        auto &v = vertices[k];
        auto &vT3D = verticesT3D[k];

        // calc. matrix, the included one seems to be always NULL (why?)
        // @TODO: dont bake this for skinned meshes!
        Mat4 matScale{};
        if(node->has_scale)matScale.setScale({node->scale[0], node->scale[1], node->scale[2]});

        Mat4 matRot{};
        if(node->has_rotation)matRot.setRot({
          node->rotation[3],
          node->rotation[0],
          node->rotation[1],
          node->rotation[2]
        });

        Mat4 matTrans{};
        if(node->has_translation)matTrans.setPos({node->translation[0], node->translation[1], node->translation[2]});

        // apply matrix and base model scale (for fixes point accuracy)
        Mat4 mat = matTrans * matRot * matScale;
        auto posInt = mat * v.pos * modelScale;

        posInt = posInt.round();
        vT3D.pos[0] = (int16_t)posInt.x();
        vT3D.pos[1] = (int16_t)posInt.y();
        vT3D.pos[2] = (int16_t)posInt.z();

        auto normPacked = (v.norm * Vec3{15.5f, 31.5f, 15.5f})
          .round()
          .clamp(
            Vec3{-16.0f, -32.0f, -16.0f},
            Vec3{ 15.0f,  31.0f,  15.0f}
          );

        vT3D.norm = ((int16_t)(normPacked[0]) & 0b11111 ) << 11
                  | ((int16_t)(normPacked[1]) & 0b111111) <<  5
                  | ((int16_t)(normPacked[2]) & 0b11111 ) <<  0;

        vT3D.rgba = 0;
        vT3D.rgba |= (uint32_t)(v.color[3] * 255.0f) << 0;
        vT3D.rgba |= (uint32_t)(v.color[2] * 255.0f) << 8;
        vT3D.rgba |= (uint32_t)(v.color[1] * 255.0f) << 16;
        vT3D.rgba |= (uint32_t)(v.color[0] * 255.0f) << 24;

        vT3D.s = (int16_t)(int32_t)(v.uv[0] * texSizeX * 32.0f);
        vT3D.t = (int16_t)(int32_t)(v.uv[1] * texSizeY * 32.0f);

        vT3D.s -= 16.0f; // bi-linear offset (@TODO: do this in ucode?)
        vT3D.t -= 16.0f;

        // Generate hash for faster lookup later in the optimizer
        vT3D.hash = ((uint64_t)(uint16_t)vT3D.pos[0] << 48)
                  | ((uint64_t)(uint16_t)vT3D.pos[1] << 32)
                  | ((uint64_t)(uint16_t)vT3D.pos[2] << 16)
                  | ((uint64_t)vT3D.norm << 0);
        vT3D.hash ^= ((uint64_t)vT3D.rgba) << 5;
        vT3D.hash ^= ((uint64_t)(uint16_t)vT3D.s << 16)
                   | ((uint64_t)(uint16_t)vT3D.t << 0);
      }

      // optimizations
      meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), vertices.size());
      //meshopt_optimizeOverdraw(indices.data(), indices.data(), indices.size(), &vertices[0].pos.data[0], vertices.size(), sizeof(VertexNorm), 1.05f);

      // expand into triangles, this is used to split up and dedupe data
      model.triangles.reserve(indices.size() / 3);

      for(int k = 0; k < indices.size(); k += 3)
      {
        u32 idxA = indices[k + 0];
        u32 idxB = indices[k + 1];
        u32 idxC = indices[k + 2];

        model.triangles.push_back({
          verticesT3D[idxA],
          verticesT3D[idxB],
          verticesT3D[idxC],
        });
      }
    }
  }

  cgltf_free(data);
  return models;
}
