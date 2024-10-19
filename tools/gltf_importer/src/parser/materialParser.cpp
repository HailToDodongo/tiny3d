/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "parser.h"
#include "../hash.h"
#include "./rdp.h"
#include "../fast64Types.h"

#include "../lib/lodepng.h"
#include "../lib/json.hpp"
using json = nlohmann::json;

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

  void readMaterialFromJson(MaterialTexture &material, const json &tex, const fs::path &gltfPath)
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
      //printf("Loaded Texture %s, size: %dx%d\n", material.texPath.c_str(), material.texWidth, material.texHeight);
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

  void readColor(const json &color, uint8_t out[4]) {
    float colorFloat[4] = {
      color[0].get<float>(),
      color[1].get<float>(),
      color[2].get<float>(),
      color[3].get<float>()
    };

    // linear to gamma
    for(int c=0; c<3; ++c) {
      colorFloat[c] = powf(colorFloat[c], 0.4545f);
    }

    out[0] = (uint8_t)(colorFloat[0] * 255.0f);
    out[1] = (uint8_t)(colorFloat[1] * 255.0f);
    out[2] = (uint8_t)(colorFloat[2] * 255.0f);
    out[3] = (uint8_t)(colorFloat[3] * 255.0f);
  }
}

void parseMaterial(const fs::path &gltfBasePath, int i, int j, Model &model, cgltf_primitive *prim) {
  model.material.uuid = j * 1000 + i;
  if(prim->material->name) {
    model.material.uuid = stringHash(prim->material->name);
    model.material.name = prim->material->name;
  }
  //printf("     Material: %s\n", prim->material->name);

  if(config.ignoreMaterials) {
    printf("Ignoring material\n");
    return;
  }

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

  uint64_t otherModeValue = 0;
  uint64_t otherModeMask = RDP::SOM::ALPHA_COMPARE_MASK | RDP::SOM::SAMPLE_MASK;

  if(!f3dData.empty()) {
    //printf("  - %s\n", f3dData.dump(2).c_str());
    //printf("  - %s\n", f3dData["combiner2"].dump(2).c_str());

    auto cc1 = readCCFromJson(f3dData["combiner1"]);
    auto cc2 = readCCFromJson(f3dData["combiner2"]);
    bool is2Cycle = true;

    model.material.drawFlags = DrawFlags::DEPTH;
    model.material.blendColor[3] = 128; // default in case cutout is used

    model.material.setPrimColor = false;
    if(f3dData.contains("set_prim")) {
      model.material.setPrimColor = f3dData["set_prim"].get<uint32_t>() != 0;
      readColor(f3dData["prim_color"], model.material.primColor);
    }

    if(f3dData.contains("set_env")) {
      model.material.setEnvColor = f3dData["set_env"].get<uint32_t>() != 0;
      readColor(f3dData["env_color"], model.material.envColor);
    }

    if(f3dData.contains("set_blend")) {
      model.material.setBlendColor = f3dData["set_blend"].get<uint32_t>() != 0;
      readColor(f3dData["blend_color"], model.material.blendColor);
    }

    if(f3dData.contains("rdp_settings"))
    {
      auto &rdpSettings = f3dData["rdp_settings"];
      is2Cycle = rdpSettings["g_mdsft_cycletype"].get<uint32_t>() != 0;

      if(rdpSettings["g_cull_back"].get<uint32_t>() != 0) {
        model.material.drawFlags |= DrawFlags::CULL_BACK;
      }
      if(rdpSettings["g_cull_front"].get<uint32_t>() != 0) {
        model.material.drawFlags |= DrawFlags::CULL_FRONT;
      }

      model.material.fogMode = rdpSettings["g_fog"].get<uint32_t>() + 1;

      uint32_t texFilter = rdpSettings["g_mdsft_text_filt"].get<uint32_t>() & 0b11;
      uint64_t textFilterMap[3] = {
          RDP::SOM::SAMPLE_POINT,
          RDP::SOM::SAMPLE_MEDIAN,
          RDP::SOM::SAMPLE_BILINEAR
      };
      otherModeValue |= textFilterMap[texFilter];

      model.material.uvFilterAdjust = texFilter != 0;

      uint32_t texGen = rdpSettings["g_tex_gen"].get<uint32_t>();
      model.material.vertexFxFunc = (texGen != 0) ? UvGenFunc::SPHERE : UvGenFunc::NONE;

      /*uint32_t alphaComp = rdpSettings["g_mdsft_alpha_compare"].get<uint32_t>();
      if(alphaComp == 1) {
        otherModeValue |= RDP::SOM::ALPHA_COMPARE;
      }*/

      bool setRenderMode = rdpSettings["set_rendermode"].get<uint32_t>() != 0;
      if(setRenderMode) {
        otherModeMask |= RDP::SOM::ZMODE_MASK; // | RDP::SOM::BLALPHA_MASK;

        int renderMode1Raw = rdpSettings["rendermode_preset_cycle_1"].get<uint32_t>();
        int renderMode2Raw = rdpSettings["rendermode_preset_cycle_2"].get<uint32_t>();
        uint32_t blenderMode1 = F64_RENDER_MODE_1_TO_BLENDER[renderMode1Raw];
        uint32_t blenderMode2 = F64_RENDER_MODE_2_TO_BLENDER[renderMode2Raw];

        auto otherMode1 = F64_RENDER_MODE_1_TO_OTHERMODE[renderMode1Raw];
        auto otherMode2 = F64_RENDER_MODE_2_TO_OTHERMODE[renderMode2Raw];

        otherModeValue |= otherMode1 | otherMode2;

        /*if(rdpSettings["cvg_x_alpha"].get<uint32_t>()) {
          otherModeValue |= RDP::SOM::BLALPHA_CVG_X_CC | RDP::SOM::BLALPHA_CVG;
        }*/

        model.material.blendMode = is2Cycle ? blenderMode2 : blenderMode1;

      } else {
        // if no render mode is set, we need to check the draw layer
        uint32_t layerOOT = f3dData["draw_layer"].contains("oot") ? f3dData["draw_layer"]["oot"].get<uint32_t>() : 0;
        uint32_t layerSM64 = f3dData["draw_layer"].contains("sm64") ? f3dData["draw_layer"]["sm64"].get<uint32_t>() : 0;

        // since we don't know what game was set, choose the non-zero one,
        // or if both set (impossible?) use the higher one
        if(layerOOT > layerSM64) {
          switch(layerOOT) {
            default: // has only 3 distinct layers:
            case 0: model.material.blendMode = RDP::BLEND::NONE; break; // Opaque
            case 1: model.material.blendMode = RDP::BLEND::MULTIPLY; break; // Transparent
            case 2:
              model.material.blendMode = RDP::BLEND::NONE;
              otherModeValue |= RDP::SOM::ALPHA_COMPARE;
            break; // Overlay
          }
        } else {
          // has multiple layers with variants (e.g. intersecting) ignore the finer details here:
          if(layerSM64 <= 1) {
            model.material.blendMode = RDP::BLEND::NONE;
          } else if(layerSM64 <= 4) {
            model.material.blendMode = RDP::BLEND::NONE;
            otherModeValue |= RDP::SOM::ALPHA_COMPARE;
          } else {
            model.material.blendMode = RDP::BLEND::MULTIPLY;
          }
        }
      }
    }

    if(model.material.fogMode == FogMode::ACTIVE || isUsingShade(cc1) || (is2Cycle && isUsingShade(cc2))) {
      model.material.drawFlags |= DrawFlags::SHADED;
    }

    if(isCCUsingTexture(cc1) || (is2Cycle && isCCUsingTexture(cc2))) {
      model.material.drawFlags |= DrawFlags::TEXTURED;

      if(f3dData.contains("tex0"))readMaterialFromJson(model.material.texA, f3dData["tex0"], gltfBasePath);
      if(f3dData.contains("tex1"))readMaterialFromJson(model.material.texB, f3dData["tex1"], gltfBasePath);
    }

    if(is2Cycle) {
      model.material.colorCombiner  = RDPQ_COMBINER_2PASS |
        rdpq_2cyc_comb2a_rgb(cc1.a, cc1.b, cc1.c, cc1.d) |
        rdpq_2cyc_comb2a_alpha(cc1.aAlpha, cc1.bAlpha, cc1.cAlpha, cc1.dAlpha) |
        rdpq_2cyc_comb2b_rgb(cc2.a, cc2.b, cc2.c, cc2.d) |
        rdpq_2cyc_comb2b_alpha(cc2.aAlpha, cc2.bAlpha, cc2.cAlpha, cc2.dAlpha);
    } else {
      model.material.colorCombiner  =
        rdpq_1cyc_comb_rgb(cc1.a, cc1.b, cc1.c, cc1.d) |
        rdpq_1cyc_comb_alpha(cc1.aAlpha, cc1.bAlpha, cc1.cAlpha, cc1.dAlpha);
    }

  } else {
    printf("No Fast64 Material data found!\n");
  }

  model.material.otherModeValue = otherModeValue;
  model.material.otherModeMask = otherModeMask;
}