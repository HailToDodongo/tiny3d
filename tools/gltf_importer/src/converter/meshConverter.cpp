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

void convertVertex(
  float modelScale, float texSizeX, float texSizeY, const VertexNorm &v, VertexT3D &vT3D,
  const Mat4 &mat, const std::vector<Mat4> &matrices, bool uvAdjust
) {
  //auto posInt = mat * v.pos * modelScale;
  auto posInt = v.pos;

  Mat4 normMat = mat;
  Vec3 norm = v.norm;
  if(v.boneIndex >= 0) {
    // pre-transform position into bone space
    auto boneMat = matrices[v.boneIndex];
    posInt = boneMat * (posInt);

    normMat = boneMat * normMat;
  }

  normMat[3] = Vec4{0.0f, 0.0f, 0.0f, 1.0f};
  norm = (normMat * norm).normalize();

  posInt = mat * posInt * modelScale;
  posInt = (posInt).round();
  vT3D.pos[0] = (int16_t)posInt.x();
  vT3D.pos[1] = (int16_t)posInt.y();
  vT3D.pos[2] = (int16_t)posInt.z();

  auto normPacked = (norm * Vec3{15.5f, 31.5f, 15.5f})
    .round()
    .clamp(
      Vec3{-16.0f, -32.0f, -16.0f},
      Vec3{ 15.0f,  31.0f,  15.0f}
    );

  vT3D.norm = ((int16_t)(normPacked[0]) & 0b11111 ) << 11
            | ((int16_t)(normPacked[1]) & 0b111111) <<  5
            | ((int16_t)(normPacked[2]) & 0b11111 ) <<  0;

  vT3D.rgba = (uint32_t)(v.color[3] * 255.0f);
  vT3D.rgba |= (uint32_t)(v.color[2] * 255.0f) << 8;
  vT3D.rgba |= (uint32_t)(v.color[1] * 255.0f) << 16;
  vT3D.rgba |= (uint32_t)(v.color[0] * 255.0f) << 24;

  // Enable this to debug bone-indices:
  /*if(v.boneIndex >= 0) {
    vT3D.rgba = 0xFF;
    vT3D.rgba |= (uint32_t)((uint32_t)((v.boneIndex+1) * 180) % 256) << 8;
    vT3D.rgba |= (uint32_t)((uint32_t)((v.boneIndex+1) * 80) % 256) << 16;
    vT3D.rgba |= (uint32_t)((uint32_t)((v.boneIndex+1) * 50) % 256) << 24;
  } else {
    vT3D.rgba = 0xFFFFFFFF;
  }*/

  vT3D.s = (int16_t)(int32_t)(v.uv[0] * texSizeX * 32.0f);
  vT3D.t = (int16_t)(int32_t)(v.uv[1] * texSizeY * 32.0f);

  if(uvAdjust) {
    vT3D.s -= 16.0f;
    vT3D.t -= 16.0f;
  }

  // Generate hash for faster lookup later in the optimizer
  vT3D.hash = ((uint64_t)(uint16_t)vT3D.pos[0] << 48)
            | ((uint64_t)(uint16_t)vT3D.pos[1] << 32)
            | ((uint64_t)(uint16_t)vT3D.pos[2] << 16)
            | ((uint64_t)vT3D.norm << 0);
  vT3D.hash ^= ((uint64_t)vT3D.rgba) << 5;
  vT3D.hash ^= ((uint64_t)(uint16_t)vT3D.s << 16)
             | ((uint64_t)(uint16_t)vT3D.t << 0);
  vT3D.hash ^= ((v.boneIndex*5) << 16) | (v.boneIndex << 24);

  vT3D.boneIndex = v.boneIndex;
}

ModelChunked chunkUpModel(const Model &model)
{
  ModelChunked res{
    .aabbMin = { 32767, 32767, 32767 },
    .aabbMax = { -32768, -32768, -32768 }
  };
  res.chunks.reserve(model.triangles.size() * 3 / MAX_VERTEX_COUNT);
  res.chunks.push_back(MeshChunk{});
  res.chunks.back().material = model.material;
  res.chunks.back().name = model.name;

  uint32_t emittedVerts = 0;
  uint32_t chunkOffset = 0;

  // Emits a new chunk of data. This contains a set of indices referencing the global vertex buffer
  auto checkAndEmitChunk = [&](bool forceEmit)
  {
     if(emittedVerts >= MAX_VERTEX_COUNT || forceEmit) {
        if(emittedVerts == 0 || res.vertices.empty())return false; // no need to emit empty chunks

        // make sure vertices can be interleaved later
        if(res.vertices.size() % 2 != 0) {
          if(!forceEmit) {
            throw std::runtime_error("Not a multiple of 2!");
          }
          res.vertices.push_back(res.vertices.back());
          ++emittedVerts;
        }

        if(emittedVerts > MAX_VERTEX_COUNT) {
          printf("Error: Too many vertices: %d (total: %d)\n", emittedVerts, res.vertices.size());
          throw std::runtime_error("Too many vertices!");
        }

        res.chunks.back().vertexCount = emittedVerts;
        res.chunks.back().vertexOffset = chunkOffset;

        // Special handling for bones: we need to sort new vertices by the bone index,
        // then split up the chunk into multiple ones, each containing only one common bone index.
        // All except the last will only load vertices, but draw no faces. The last one will do the drawing.

        // iterate over all new verts and re-collect them into buffers
        std::unordered_map<int32_t, std::vector<VertexT3D>> vertsByBone{};

        for(uint32_t v=chunkOffset; v<(chunkOffset+emittedVerts); ++v) {
          auto &vert = res.vertices[v];
          vert.originalIndex = v - chunkOffset;
          vertsByBone[vert.boneIndex].push_back(vert);
        }

        // if we only have one bone (can also mean no bones at all) -> do nothing
        if(vertsByBone.size() > 1)
        {
          auto orgChunk = res.chunks.back();
          res.chunks.pop_back();

          uint32_t v=chunkOffset;
          std::vector<uint32_t> indexMap{};
          indexMap.resize(emittedVerts, 0);
          uint32_t chunkSubOffset = chunkOffset;
          uint32_t vertDestOffset = 0;

          for(auto & [boneIndex, verts] : vertsByBone) {
            // per unique bone index, create a new chunk...
            ++orgChunk.boneCount;
            auto subChunk = orgChunk;
            subChunk.vertexCount = verts.size(); // ...only for its vertices...
            subChunk.vertexOffset = chunkSubOffset; // ...starting from the last chunks offset
            subChunk.vertexDestOffset = vertDestOffset;
            subChunk.boneIndex = boneIndex;
            subChunk.indices.clear();

            for(auto &vert : verts) {
              res.vertIdxMap[vert.hash] = v;
              res.vertices[v++] = vert;
              indexMap[vert.originalIndex] = vertDestOffset++;
            }

            // if out vertex count is odd, inject a dummy vertex to keep alignment
            // this will only affect the buffer that's read from, the target buffer on the RSP will have the real index
            if(verts.size() % 2 != 0) {
              subChunk.vertexCount += 1;

              // now inject a dummy vertex at 'chunkSubOffset' into the input buffer to keep alignment
              res.vertices.insert(res.vertices.begin() + v, res.vertices.back());
              ++emittedVerts;
              ++v;
            }

            res.chunks.push_back(subChunk);
            chunkSubOffset += subChunk.vertexCount;
          }

          // re-assign the indices in the last chunk that does the drawing
          res.chunks.back().indices = orgChunk.indices;

          for(auto &idx : res.chunks.back().indices) {
            idx = indexMap[idx];
          }
        } else {
          // chunk could still have a bone assignment, grab the bone index from the first vertex
          if(!vertsByBone.empty()) {
            res.chunks.back().boneIndex = vertsByBone.begin()->first;
          }
        }

        res.chunks.push_back(MeshChunk{.material = model.material, .name = model.name});

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

    // check if triangle would still fit into the buffer
    if((emittedVerts + needsEmit.size()) >= MAX_VERTEX_COUNT) {
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
      auto freeVertLeft = MAX_VERTEX_COUNT - emittedVerts;
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
    return chunk.vertexCount == 0;
  }), res.chunks.end());

  // check validity
  assert(res.vertices.size() % 2 == 0);
  for(const auto &chunk : res.chunks) {
    assert(chunk.vertexCount % 2 == 0);
    assert(chunk.vertexOffset % 2 == 0);
    // 'chunk.vertexDestOffset' needs no alignment

    // we can go a little bit OOB (there is a tmp buffer after it, and the DMA doesn't overlap)
    // this may be needed to split vertices with bones properly
    assert((chunk.vertexDestOffset + chunk.vertexCount) <= (MAX_VERTEX_COUNT+1));
  }

  // calculate AABB
  for(const auto &v : res.vertices) {
    res.aabbMin[0] = std::min(res.aabbMin[0], v.pos[0]);
    res.aabbMin[1] = std::min(res.aabbMin[1], v.pos[1]);
    res.aabbMin[2] = std::min(res.aabbMin[2], v.pos[2]);

    res.aabbMax[0] = std::max(res.aabbMax[0], v.pos[0]);
    res.aabbMax[1] = std::max(res.aabbMax[1], v.pos[1]);
    res.aabbMax[2] = std::max(res.aabbMax[2], v.pos[2]);
  }

  return res;
}
