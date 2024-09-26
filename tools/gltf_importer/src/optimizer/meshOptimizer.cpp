/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "optimizer.h"
#include "../lib/meshopt/meshoptimizer.h"
#include <cassert>
#include <array>

// NOTE: mesh optimizations prior to chunking the model up are done in parser.cpp via 'meshopt_optimizeVertexCache'

namespace {
  std::array<int, 2> getSharedIndex(std::array<uint8_t, 3> &triA, std::array<uint8_t, 3> &triB) {
    for(int i=0; i<3; ++i) {
      for(int j=0; j<3; ++j) {
        if(triA[i] == triB[j])return {i, j};
      }
    }
    return {-1, -1};
  }

  void arrayShiftRight(std::array<uint8_t, 3> &arr, int shift) {
    for(int i=0; i<shift; ++i) {
      std::swap(arr[0], arr[1]);
      std::swap(arr[0], arr[2]);
    }
  }
}

void optimizeModelChunk(ModelChunked &model)
{
  for(auto &chunk : model.chunks)
  {
    printf("Indices: %d\n", chunk.indices.size());

    std::vector<std::array<uint8_t, 3>> tris{}; // input tris
    for(int i=0; i<chunk.indices.size(); i+=3) {
      tris.push_back({chunk.indices[i], chunk.indices[i+1], chunk.indices[i+2]});
    }

    std::vector<uint8_t> indicesFans{};
    std::vector<uint8_t> indicesRepeat{};
    chunk.indices.clear();

    // try to detect triangles that have ascending indices (e.g. [0,1,2], [3,4,5], [6,7,8], ...)
    // this can be compacted into a single triangle + a repeat count ([0,1,2] x 3)
    /*int baseIndex = 0;
    int repeatCount = 0;
    std::array<uint8_t, 3> repeatTri{0,0,0};
    for(int t=0; t<tris.size(); ++t)
    {
      bool isLastTri = t == tris.size() - 1;
      auto &tri = tris[t];
      if(!isLastTri && tri[0] == baseIndex && repeatCount < 255) {
        if(repeatCount == 0)repeatTri = tri;
        ++repeatCount;
      } else {
        if(repeatCount > 1) {
          printf("   Tri repeat: count=%d | %d %d %d\n", repeatCount, repeatTri[0], repeatTri[1], repeatTri[2]);
          indicesRepeat.push_back(repeatCount);
          indicesRepeat.push_back(repeatTri[0]);
          indicesRepeat.push_back(repeatTri[1]);
          indicesRepeat.push_back(repeatTri[2]);
          // now erase the repeated tris inb the source list
          tris.erase(tris.begin() + t - repeatCount, tris.begin() + t);
          t -= repeatCount;
        }
        repeatCount = 0;
      }

      baseIndex = tri[0] + 3;
    }*/
    {
      //assert(chunk.isStrip == false);
      std::vector<uint8_t> optimizedIndices{};
      optimizedIndices.resize((chunk.indices.size() / 3) * 5); // worst case
      size_t optimizedIndexCount = meshopt_stripify(
        optimizedIndices.data(), chunk.indices.data(), chunk.indices.size(), chunk.vertexCount, (uint8_t)0xFF
      );
      //chunk.isStrip = true;

      printf("Old: ");
      for(size_t i=0; i<chunk.indices.size(); ++i) {
        printf("%d ", chunk.indices[i]);
      }

      printf("\nstrip indices: %d -> %d: ", chunk.indices.size(), optimizedIndexCount);
      //chunk.indices.clear();
      for(size_t i=0; i<optimizedIndexCount; ++i) {
        printf("%d ", optimizedIndices[i]);
        //chunk.indices.push_back(optimizedIndices[i]);
      }
      printf("\n");
    }

    for(int t=0; t<tris.size(); ++t)
    {
      auto &tri = tris[t];
      chunk.indices.push_back(tri[0]);
      chunk.indices.push_back(tri[1]);
      chunk.indices.push_back(tri[2]);
    }

    // try to detect triangles that are connected by at least one vertex
    // e.g. [0,1,2] [2,3,4] -> [0,1,2,3,4]
    // the input indices can be rotated to fit (shared index must be in the middle) as long as winding order is preserved
    /*for(int t=0; t<tris.size(); ++t)
    {
      bool foundQuad = false;
      auto &tri = tris[t];
      for(int r=t+1; r<tris.size(); ++r)
      {
        auto sharedIdx = getSharedIndex(tri, tris[r]);
        if(sharedIdx[0] >= 0) {
          arrayShiftRight(tri, 2-sharedIdx[0]);
          arrayShiftRight(tris[r], (3-sharedIdx[1]) % 3);
          assert(tri[2] == tris[r][0]);
          printf("   Share: idx=%d/%d | %d %d %d with %d %d %d\n", t, r, tri[0], tri[1], tri[2], tris[r][0], tris[r][1], tris[r][2]);
          indicesFans.insert(indicesFans.end(), {tri[0], tri[1], tri[2], tris[r][1], tris[r][2]});
          tris.erase(tris.begin() + r);
          tris.erase(tris.begin() + t);
          --t;
          foundQuad = true;
          break;
        }
      }
      if(!foundQuad) {
        printf("   Tri: idx=%d | %d %d %d\n", t, tri[0], tri[1], tri[2]);
        chunk.indices.push_back(tri[0]);
        chunk.indices.push_back(tri[1]);
        chunk.indices.push_back(tri[2]);
      }
    }*/

    if(!indicesFans.empty() || !indicesRepeat.empty()) {
      chunk.indices.push_back(0xFF);
      chunk.indices.insert(chunk.indices.end(), indicesFans.begin(), indicesFans.end());
      chunk.indices.push_back(0xFF);
      chunk.indices.insert(chunk.indices.end(), indicesRepeat.begin(), indicesRepeat.end());
    }
  }
}