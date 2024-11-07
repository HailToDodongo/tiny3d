/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#pragma once

#include <cstdint>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/mat4.h"

namespace DrawFlags {
  constexpr uint32_t DEPTH      = 1 << 0;
  constexpr uint32_t TEXTURED   = 1 << 1;
  constexpr uint32_t SHADED     = 1 << 2;
  constexpr uint32_t CULL_FRONT = 1 << 3;
  constexpr uint32_t CULL_BACK  = 1 << 4;
}

namespace CC {
  constexpr uint32_t COMBINED = 0;
  constexpr uint32_t TEX0     = 1;
  constexpr uint32_t TEX1     = 2;
  constexpr uint32_t PRIM     = 3;
  constexpr uint32_t SHADE    = 4;
  constexpr uint32_t ENV      = 5;
  constexpr uint32_t NOISE    = 6;
  constexpr uint32_t TEX0_ALPHA = 8;
  constexpr uint32_t TEX1_ALPHA = 9;
}

namespace FogMode {
  constexpr uint8_t DEFAULT  = 0;
  constexpr uint8_t DISABLED = 1;
  constexpr uint8_t ACTIVE   = 2;

  constexpr uint8_t INVALID = 0xFF;
}

namespace UvGenFunc {
  constexpr uint8_t NONE  = 0;
  constexpr uint8_t SPHERE = 1;
}

// Normalized vertex, this is then used to generate the final vertex data
struct VertexNorm {
  Vec3 pos{};
  Vec3 norm{};
  float color[4]{};
  Vec2 uv{};
  int32_t boneIndex{-1};
};

struct VertexT3D {
  /* 0x00 */ int16_t pos[3]{}; // 16.0 fixed point
  /* 0x06 */ uint16_t norm{}; // 5,5,5 compressed normal
  /* 0x08 */ uint32_t rgba{}; // RGBA8 color
  /* 0x0C */ int16_t s{}; // 10.6 fixed point (pixel coords)
  /* 0x0E */ int16_t t{}; // 10.6 fixed point (pixel coords)

  // Extra attributes not used in the final vertex data:
  uint64_t hash{};
  int32_t boneIndex{};
  uint32_t originalIndex{};

  bool operator==(const VertexT3D& v) const {
    return hash == v.hash;
  }

  constexpr static uint32_t byteSize() {
    return sizeof(VertexT3D) - sizeof(hash) - sizeof(boneIndex) - sizeof(originalIndex);
  }

  //bool operator<=>(const VertexT3D&) const = default;
};

static_assert(VertexT3D::byteSize() == 0x10, "VertexT3D has wrong size");

struct TriangleT3D {
  VertexT3D vert[3]{};
  //bool operator<=>(const TriangleT3D&) const = default;
};

struct TileParam {
  float low{};
  float high{};
  uint8_t clamp{};
  uint8_t mirror{};
  int8_t mask{};
  int8_t shift{};
};

struct ColorCombiner {
  uint8_t a{};
  uint8_t b{};
  uint8_t c{};
  uint8_t d{};
  uint8_t aAlpha{};
  uint8_t bAlpha{};
  uint8_t cAlpha{};
  uint8_t dAlpha{};
};

struct MaterialTexture {
  std::string texPath{};
  uint32_t texWidth{};
  uint32_t texHeight{};
  uint32_t texReference{};

  TileParam s{};
  TileParam t{};
};

struct Material {
  uint32_t index{};
  MaterialTexture texA;
  MaterialTexture texB;
  std::string name{};

  uint64_t colorCombiner{};
  uint64_t otherModeValue{};
  uint64_t otherModeMask{};
  uint32_t blendMode{};
  uint32_t drawFlags{};

  uint32_t uuid{};
  uint8_t fogMode{};
  uint8_t vertexFxFunc{};

  uint8_t primColor[4]{};
  uint8_t envColor[4]{};
  uint8_t blendColor[4]{};

  bool setPrimColor{false};
  bool setEnvColor{false};
  bool setBlendColor{false};
  bool uvFilterAdjust{false};
};

struct MeshChunk {
  std::vector<int8_t> indices{};
  std::vector<int16_t> stripIndices[4]{};
  Material material{};
  uint32_t vertexOffset{0};
  uint32_t vertexCount{0};
  uint32_t vertexDestOffset{0};
  uint32_t boneIndex{0};
  uint32_t boneCount{0};
  std::string name{};
};

struct Model {
  std::vector<TriangleT3D> triangles{};
  std::string name{};
  Material material{};
};

struct ModelChunked {
  std::vector<VertexT3D> vertices{};
  std::vector<MeshChunk> chunks{};

  std::unordered_map<uint64_t, uint32_t> vertIdxMap{};

  Material materialA{};
  Material materialB{};

  s16 aabbMin[3]{};
  s16 aabbMax[3]{};
  u16 triCount{};
};

struct Bone {
  std::string name;
  Vec3 pos;
  Quat rot;
  Vec3 scale;

  Mat4 parentMatrix;
  Mat4 modelMatrix;
  Mat4 inverseBindPose;

  uint32_t index;
  uint32_t parentIndex;
  std::vector<std::shared_ptr<Bone>> children;
};

typedef enum AnimChannelTarget : u8 {
  TRANSLATION,
  SCALE,
  SCALE_UNIFORM,
  ROTATION
} AnimChannelTarget;

struct Keyframe {
  float time{};
  float timeNeeded{};
  float timeNextInChannel{};

  uint16_t timeTicks{};
  uint16_t timeNeededTicks{};
  uint16_t timeNextInChannelTicks{};

  uint32_t chanelIdx;
  Quat valQuat;
  float valScalar;

  uint32_t valQuantSize = 0;
  uint16_t valQuant[2];
};

struct AnimChannelMapping {
  std::string targetName{};
  uint16_t targetIdx{};
  AnimChannelTarget targetType{};
  uint8_t attributeIdx{};

  float valueMin{INFINITY};
  float valueMax{-INFINITY};

  std::vector<Keyframe> keyframes{}; // temp. storage after parsing

  [[nodiscard]] constexpr bool isRotation() const {
    return targetType == AnimChannelTarget::ROTATION;
  }
};

struct Anim {
  std::string name{};
  float duration{};
  uint32_t channelCountQuat{};
  uint32_t channelCountScalar{};
  std::vector<Keyframe> keyframes{}; // output used for writing to the file
  std::vector<AnimChannelMapping> channelMap{};
};

struct T3DMData {
  std::vector<Model> models{};
  std::vector<Bone> skeletons{};
  std::vector<Anim> animations{};
};

struct Config {
  float globalScale{64.0f};
  uint32_t animSampleRate{30};
  bool ignoreMaterials{false};
  bool createBVH{false};
  bool verbose{false};
};
extern Config config;

constexpr int MAX_VERTEX_COUNT = 70;
constexpr int CACHE_VERTEX_SIZE = 36;
constexpr u8 T3DM_VERSION = 0x03;