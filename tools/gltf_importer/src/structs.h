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

#include "math/vec2.h"
#include "math/vec3.h"

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

namespace AlphaMode {
  constexpr uint8_t DEFAULT = 0;
  constexpr uint8_t OPAQUE  = 1;
  constexpr uint8_t CUTOUT  = 2;
  constexpr uint8_t TRANSP  = 3;

  constexpr uint8_t INVALID = 0xFF;
}

namespace FogMode {
  constexpr uint8_t DEFAULT  = 0;
  constexpr uint8_t DISABLED = 1;
  constexpr uint8_t ACTIVE   = 2;

  constexpr uint8_t INVALID = 0xFF;
}

// Normalized vertex, this is then used to generate the final vertex data
struct VertexNorm {
  Vec3 pos{};
  Vec3 norm{};
  float color[4]{};
  Vec2 uv{};
};


struct VertexT3D {
  /* 0x00 */ int16_t pos[3]{}; // 16.0 fixed point
  /* 0x06 */ uint16_t norm{}; // 5,5,5 compressed normal
  /* 0x08 */ uint32_t rgba{}; // RGBA8 color
  /* 0x0C */ int16_t s{}; // 10.6 fixed point (pixel coords)
  /* 0x0E */ int16_t t{}; // 10.6 fixed point (pixel coords)

  uint64_t hash{};

  bool operator==(const VertexT3D& v) const {
    return hash == v.hash;
  }

  constexpr static uint32_t byteSize() {
    return sizeof(VertexT3D) - sizeof(hash);
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

struct Material {
  TileParam s{};
  TileParam t{};
  uint64_t colorCombiner{};
  uint32_t drawFlags{};
  std::string texPath{};
  uint32_t texWidth{};
  uint32_t texHeight{};
  uint32_t texReference{};
  uint32_t uuid{};
  uint8_t alphaMode{};
  uint8_t fogMode{};
};

struct MeshChunk {
  std::vector<uint8_t> indices{};
  Material materialA{};
  Material materialB{};
  uint32_t vertexOffset{0};
  uint32_t vertexCount{0};
};

struct Model {
  std::vector<TriangleT3D> triangles{};
  Material materialA{};
  Material materialB{};
};

struct ModelChunked {
  std::vector<VertexT3D> vertices{};
  std::vector<MeshChunk> chunks{};

  std::unordered_map<uint64_t, uint32_t> vertIdxMap{};

  Material materialA{};
  Material materialB{};
};