/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "optimizer.h"
#include <algorithm>
#include <array>

#include "../lib/tristrip/tri_stripper.h"

// NOTE: mesh optimizations prior to chunking the model up are done in parser.cpp via 'meshopt_optimizeVertexCache'

namespace {
  typedef std::array<int8_t, 3> Tri;
  typedef std::vector<Tri> TriList;

  bool triHasIndex(const Tri &tri, int idx) {
    return tri[0] == idx || tri[1] == idx || tri[2] == idx;
  }

  // get amount of times each vertex is used in the input triangle list
  auto getVertexUsage(const TriList &tris) {
    std::array<int, MAX_VERTEX_COUNT> usedVerts{};
    for(auto &tri : tris) {
      for(int i=0; i<3; ++i) {
        ++usedVerts[tri[i]];
      }
    }
    return usedVerts;
  }

  auto getVertexUsage(const std::vector<std::vector<int8_t>> strips) {
    std::array<int, MAX_VERTEX_COUNT> usedVerts{};
    for(auto &strip : strips) {
      for(auto idx : strip) {
        ++usedVerts[idx];
      }
    }
    return usedVerts;
  }

  int countFreeVertsAtEnd(const std::array<int, MAX_VERTEX_COUNT> &usedVerts) {
    int freeVertsEnd = 0;
    for(int i=MAX_VERTEX_COUNT-1; i>=0; --i) {
      if(usedVerts[i] > 0)break;
      ++freeVertsEnd;
    }
    return freeVertsEnd;
  }

  int calcUsableIndices(int freeVertices) {
    int res = freeVertices * CACHE_VERTEX_SIZE / 2;
    // a single vertex is not aligned (2 are), sub. 8 bytes to not overwrite stuff
    if(freeVertices % 2 != 0)res -= 8 / 2;
    return res;
  }

  // stripify the input triangle list into multiple strips
  std::vector<std::vector<int8_t>> stripify(const TriList& tris, int vertexCount)
  {
    std::vector<std::vector<int8_t>> res{};
    triangle_stripper::primitive_vector PrimitivesVector{};
    std::vector<triangle_stripper::index> indices{};
    for(auto &tri : tris) {
      indices.push_back(tri[0]);
      indices.push_back(tri[1]);
      indices.push_back(tri[2]);
    }
    triangle_stripper::tri_stripper TriStripper(indices);

		TriStripper.SetMinStripSize(2);
		TriStripper.SetCacheSize(0);
		TriStripper.SetBackwardSearch(false); // seems to be broken(?)
		TriStripper.Strip(&PrimitivesVector);

		for(auto &v : PrimitivesVector) {

		  if(v.Type == triangle_stripper::primitive_type::TRIANGLES)  {
		    for(int i=0; i<v.Indices.size(); i+=3) {
		      res.push_back({(int8_t)v.Indices[i], (int8_t)v.Indices[i+1], (int8_t)v.Indices[i+2]});
		    }
      } else {
        std::vector<int8_t> strip{};
        for(auto idx : v.Indices) {
          strip.push_back(idx);
        }
        res.push_back(strip);
      }
		}
		// sort res by the highest index inside of the individual array
		std::sort(res.begin(), res.end(), [](const std::vector<int8_t> &a, const std::vector<int8_t> &b) {
      auto minA = *std::max_element(a.begin(), a.end());
      auto minB = *std::max_element(b.begin(), b.end());
      return minA < minB;
    });

		return res;
  }

  std::vector<int8_t> destripify(const std::vector<int8_t> &strip)
  {
    std::vector<int8_t> res{};
    for(int i=0; i<strip.size()-2; ++i) {
      if(strip[i] == strip[i+2])continue;
      if(i % 2 == 0) {
        res.push_back(strip[i]);
        res.push_back(strip[i+1]);
        res.push_back(strip[i+2]);
      } else {
        res.push_back(strip[i+2]);
        res.push_back(strip[i+1]);
        res.push_back(strip[i]);
      }
    }

    return res;
  }
}

void optimizeModelChunk(ModelChunked &model)
{
  for(auto &chunk : model.chunks)
  {
    // avoid skinned mesh parts with bones, these use partial loads and indices
    // which mess up the used index detection (@TODO: handle this)
    if(chunk.boneCount > 0)continue;

    // convert indices into split up triangles, then clear old indices
    TriList tris{}; // input tris
    for(int i=0; i<chunk.indices.size(); i+=3) {
      tris.push_back({chunk.indices[i], chunk.indices[i+1], chunk.indices[i+2]});
    }
    chunk.indices.clear();

    // writes out a single triangle (3 indices ina command, no DMAs)
    auto emitTri = [&chunk](const Tri &tri) {
      chunk.indices.push_back(tri[0]);
      chunk.indices.push_back(tri[1]);
      chunk.indices.push_back(tri[2]);
    };

    // emits and appends a triangle strip (1 command, DMAs data)
    auto emitStrip = [&chunk](const std::vector<int8_t> &strip, int stripIdx) {
      auto &res = chunk.stripIndices[stripIdx];
      int oldSize = res.size();
      if(!res.empty()) {
        res.push_back(strip[0] | (1<<15));
        res.insert(res.end(), strip.begin()+1, strip.end());
      } else {
        res.insert(res.end(), strip.begin(), strip.end());
      }
      return (int)(res.size() - oldSize);
    };

    // emits regular triangles until a given index is no longer used (aka free up a vertex slot)
    auto freeVertexUsage = [&tris, &emitTri](int idx) {
      for(int i=0; i<tris.size(); ++i) {
        if(triHasIndex(tris[i], idx)) {
          emitTri(tris[i]);
          tris.erase(tris.begin() + i);
          --i;
        }
      }
    };

    // Strip encoding:
    // First we stripify the entire triangle array, resulting in an array of strips.
    // Since we have to fight with the vertex cache for DMEM, we need to make sure that the strips we write don't get too big.
    // To do so we check how many vertices at the end of the buffer are unused and try to fit as many strips in there.
    // After that is done we re-check how much space is free now, and repeat the process.
    // To have a bit of space initially, we emit individual triangles until we have at least X vertices free.

    // check how many vertex slots we are free to use (end of the buffer)
    auto vertUsage = getVertexUsage(tris);
    int freeVertsEnd = countFreeVertsAtEnd(vertUsage);
    int targetFreeVerts = 2;

    // now free until the last slots are free
    if(freeVertsEnd < targetFreeVerts) {
      for(int i=MAX_VERTEX_COUNT-targetFreeVerts; i<MAX_VERTEX_COUNT; ++i) {
        freeVertexUsage(i);
      }
      vertUsage = getVertexUsage(tris);
      freeVertsEnd = countFreeVertsAtEnd(vertUsage);
    }

    int freeIndices = calcUsableIndices(freeVertsEnd);

    auto stipChunks = stripify(tris, chunk.vertexCount);
    std::reverse(stipChunks.begin(), stipChunks.end()); // puts larger indices first

    // don't do any fancy algorithms here, we want to prefer the first entries (larger indices) in order
    // to free up as much vertex space. meaning the next batch has way more space to work with
    for(int s=0; s<4 && !stipChunks.empty(); ++s)
    {
      // check if it makes sense to emit the strip
      // too few triangles are slower as strips than as regular triangles
      uint32_t totalStripIndices = 0;
      for(auto &strip : stipChunks) {
        totalStripIndices += strip.size();
      }
      if(totalStripIndices < 7)break;

      // if there are enough indices, emit the strip
      for(size_t i=0; i<stipChunks.size(); ++i) {
        int size = (int)stipChunks[i].size()+1;
        if(freeIndices - size >= 0) {
          /*printf("Emitting strip %d, space: %d: ", i, freeIndices);
          for(auto idx : stipChunks[i])printf(" %d", idx);
          printf("\n");*/
          freeIndices -= emitStrip(stipChunks[i], s);
          stipChunks.erase(stipChunks.begin() + i);
          --i;
        }
      }

      vertUsage = getVertexUsage(stipChunks);
      freeVertsEnd = countFreeVertsAtEnd(vertUsage);
      freeIndices = calcUsableIndices(freeVertsEnd);
      //printf("strip indices: %d\n", chunk.stripIndices[s].size());
    }

    // if we have some triangles left, de-stripify them and emit regular triangles
    if(!stipChunks.empty()) {
      for(auto &strip : stipChunks) {
        auto indices = destripify(strip);
        chunk.indices.insert(chunk.indices.end(), indices.begin(), indices.end());
      }
    }
  }
}