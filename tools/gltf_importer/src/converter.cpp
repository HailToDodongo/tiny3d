/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include <algorithm>
#include <cstdio>
#include <cassert>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include "converter.h"

namespace
{
  constexpr int MAX_VERTEX_COUNT_RAW = 64;
  constexpr uint16_t INVALID_INDEX = 0xFFFF;

  uint16_t getVertexIndex(const ModelChunked &model, const VertexT3D &v, uint16_t startIndex)
  {
    auto idxMaybe = model.vertIdxMap.find(v.hash);
    if(idxMaybe != model.vertIdxMap.end()) {
      auto idx = idxMaybe->second;
      if(idx >= startIndex)return idx;
    }

    return 0xFFFF;
  }

  uint16_t emitVertex(ModelChunked &model, const VertexT3D &v)
  {
    auto oldIdx = model.vertices.size();
    model.vertices.push_back(v);
    model.vertIdxMap[v.hash] = oldIdx;
    return oldIdx;
  }

  int triConnectionCount(const TriangleT3D &tri, const ModelChunked &model, uint32_t chunkOffset)
  {
    int connCount = 0;
    for(const auto & vi : tri.vert) {
      if(getVertexIndex(model, vi, chunkOffset) != INVALID_INDEX) {
        ++connCount;
      }
    }
    return connCount;
  }
}

ModelChunked chunkUpModel(const Model &model)
{
  ModelChunked res{};
  res.chunks.reserve(model.triangles.size() * 3 / MAX_VERTEX_COUNT_RAW);
  res.chunks.push_back(MeshChunk{});
  res.chunks.back().materialA = model.materialA;
  res.chunks.back().materialB = model.materialB;

  uint32_t emittedVerts = 0;
  uint32_t chunkOffset = 0;

  // Emits a new chunk of data. This contains a set of indices referencing the global vertex buffer
  auto checkAndEmitChunk = [&](bool forceEmit)
  {
     if(emittedVerts >= MAX_VERTEX_COUNT_RAW || forceEmit) {
        if(emittedVerts == 0 || res.vertices.empty())return false; // no need to emit empty chunks

        // make sure vertices can be interleaved later
        if(res.vertices.size() % 2 != 0) {
          if(!forceEmit) {
            throw std::runtime_error("Not a multiple of 2!");
          }
          res.vertices.push_back({});
          ++emittedVerts;
        }

        if(emittedVerts > MAX_VERTEX_COUNT_RAW) {
          printf("Error: Too many vertices: %d (total: %d)\n", emittedVerts, res.vertices.size());
          throw std::runtime_error("Too many vertices!");
        }

        res.chunks.back().vertexCount = emittedVerts;
        res.chunks.back().vertexOffset = chunkOffset;
        res.chunks.push_back(MeshChunk{});

        res.chunks.back().materialA = model.materialA;
        res.chunks.back().materialB = model.materialB;

        chunkOffset += emittedVerts;
        emittedVerts = 0;
        return true;
    }
    return false;
  };

  // Emit a single triangle into the local buffer
  auto emitTriangle = [&](const TriangleT3D &tri, bool onlyExisting)
  {
    // try to get existing indices in the local buffer
    uint16_t idx[3];
    std::vector<uint16_t> needsEmit{};
    for(int i=0; i<3; ++i)
    {
      idx[i] = getVertexIndex(res, tri.vert[i], chunkOffset);
      if(idx[i] == INVALID_INDEX) {
        if(onlyExisting)return false;
        needsEmit.push_back(i);
      }
    }

    if((emittedVerts + needsEmit.size()) >= MAX_VERTEX_COUNT_RAW) {
      //printf("Warning: Skipping triangle, not enough space for vertices!\n");
      return false;
    }

    // now emit missing ones (do this here to be able to skip with 'onlyExisting')
    for(auto i : needsEmit) {
      idx[i] = emitVertex(res, tri.vert[i]);
      ++emittedVerts;
    }

    // store local indices in the chunk
    res.chunks.back().indices.push_back(idx[0] - chunkOffset);
    res.chunks.back().indices.push_back(idx[1] - chunkOffset);
    res.chunks.back().indices.push_back(idx[2] - chunkOffset);

    return true;
  };

  std::vector<bool> triangleIsEmitted{};
  triangleIsEmitted.resize(model.triangles.size(), false);

  // Now we want to emit vertices and indices by iterating over the triangles.
  // We start with the most connected triangles and emit any other triangle
  // that is constructable with the current vertices.
  // This should lead to less duplicated vertices / loads.
  for(int t=0; t<model.triangles.size(); ++t)
  {
    if(triangleIsEmitted[t])continue;

    int vertsLeft = MAX_VERTEX_COUNT_RAW - emittedVerts;

    checkAndEmitChunk(false);
    if(!emitTriangle(model.triangles[t], false)) {

      if(emittedVerts % 2 != 0) {
        //printf("Tri doesn't fit, buffer % 2 != 0, emit 1 random vertex\n");
        auto &nextTri = model.triangles[(t+1) < model.triangles.size() ? (t+1) : t];
        emitVertex(res, nextTri.vert[0]);
        ++emittedVerts;

        // since we had to emit a random vertex, try again to find a fitting triangle
        for(int s=t+1; s<model.triangles.size(); ++s) {
          if(triangleIsEmitted[s])continue;
          if(emitTriangle(model.triangles[s], true)) {
            triangleIsEmitted[s] = true;
          }
        }
      }

      checkAndEmitChunk(true);
      --t;
      continue;
    }

    triangleIsEmitted[t] = true;

    // Check all other triangles that don't need new vertices
    for(int s=t+1; s<model.triangles.size(); ++s) {
      if(triangleIsEmitted[s])continue;
      if(emitTriangle(model.triangles[s], true)) {
        //log_debug("Emitting (no new): %d/%d | %d\n", s, t, triangles.size());
        triangleIsEmitted[s] = true;
      }
    }

    std::vector<int> connCounts{};
    connCounts.resize(model.triangles.size(), -1);

    // Now check the ones that have vertices in common.
    // First check 3 (no new vertex needed), then the ones with 2, then 1
    for(int maxCount=3; maxCount>0; --maxCount)
    {
      auto freeVertLeft = MAX_VERTEX_COUNT_RAW - emittedVerts;
      if(freeVertLeft < maxCount)break;

      for(int triIdx= t + 1; triIdx < model.triangles.size(); ++triIdx)
      {
        if(triangleIsEmitted[triIdx])continue;
        const auto &triCheck = model.triangles[triIdx];

        int connCount = connCounts[triIdx];
        if(connCount < 0) {
          connCount = triConnectionCount(triCheck, res, chunkOffset);
          connCounts[triIdx] = connCount;
        }
        if(connCount < maxCount)continue;

        //log_debug("Emitting (common %d): %d/%d | %d\n", connCount, s, t, triangles.size());
        if(emitTriangle(model.triangles[triIdx], false)) {
          std::fill(connCounts.begin(), connCounts.end(), -1);

          checkAndEmitChunk(false);
          triangleIsEmitted[triIdx] = true;
        }
      }
    }
  }

  checkAndEmitChunk(true);

  // remove empty chunks
  res.chunks.erase(std::remove_if(res.chunks.begin(), res.chunks.end(), [](const MeshChunk &chunk) {
    return chunk.indices.empty();
  }), res.chunks.end());

  assert(res.vertices.size() % 2 == 0);

  return res;
}
