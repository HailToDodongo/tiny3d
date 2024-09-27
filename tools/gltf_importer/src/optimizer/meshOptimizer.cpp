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

    std::vector<uint8_t> indicesRepeat{};
    std::vector<uint8_t> indicesStrip{};
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
      std::vector<uint8_t> optimizedIndices{};
      optimizedIndices.resize(tris.size() * 5); // worst case
      size_t optimizedIndexCount = meshopt_stripify(
        optimizedIndices.data(), (uint8_t*)tris[0].data(), tris.size() * 3, chunk.vertexCount, (uint8_t)0xFF
      );

      printf("Old:\n");
      for(auto &tri: tris) {
        printf("    [%d %d %d]\n", tri[0], tri[1], tri[2]);
      }

      printf("\nstrip indices: %d -> %d: ", tris.size() * 3, optimizedIndexCount);
      //chunk.indices.clear();
      for(size_t i=0; i<optimizedIndexCount; ++i) {
        printf("%d ", optimizedIndices[i]);
        indicesStrip.push_back(optimizedIndices[i]);
      }
      printf("\n");
      tris.clear();
    }

    for(int t=0; t<tris.size(); ++t)
    {
      auto &tri = tris[t];
      chunk.indices.push_back(tri[0]);
      chunk.indices.push_back(tri[1]);
      chunk.indices.push_back(tri[2]);
    }

    if(!indicesRepeat.empty()) {
      chunk.indices.push_back(0xFF);
      chunk.indices.insert(chunk.indices.end(), indicesRepeat.begin(), indicesRepeat.end());
    }

    chunk.indices.insert(chunk.indices.end(), indicesStrip.begin(), indicesStrip.end());

  }
}