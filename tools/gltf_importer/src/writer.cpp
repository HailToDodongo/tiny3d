/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <stdio.h>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cassert>

#include "structs.h"
#include "hash.h"
#include "args.h"

#include "binaryFile.h"
#include "converter/converter.h"
#include "optimizer/optimizer.h"

namespace fs = std::filesystem;

namespace {
  uint32_t insertString(std::string &stringTable, const std::string &newString) {
    auto strPos = stringTable.find(newString + '\0');
    if(strPos == std::string::npos) {
      strPos = stringTable.size();
      stringTable += newString;
      stringTable.push_back('\0');
    }
    return strPos;
  }

  int writeBone(BinaryFile &file, const T3DM::Bone &bone, std::string &stringTable, float globalScale, int level) {
    //printf("Bone[%d]: %s -> %d\n", bone.index, bone.name.c_str(), bone.parentIndex);

    file.write(insertString(stringTable, bone.name));
    file.write<uint16_t>(bone.parentIndex);
    file.write<uint16_t>(level); // level

    auto normPos = bone.pos * globalScale;
    file.writeArray(bone.scale.data, 3);
    file.writeArray(bone.rot.data, 4);
    file.writeArray(normPos.data, 3);

    int boneCount = 1;
    for(const auto& child : bone.children) {
      boneCount += writeBone(file, *child, stringTable, globalScale, level+1);
    }
    return boneCount;
  };

  std::string getRomPath(const std::string &path) {
    std::string basDir = "filesystem/";
    auto fsPos = path.find(basDir);
    if(fsPos != std::string::npos) {
      return std::string("rom:/") + path.substr(fsPos + basDir.length());
    }
    return path;
  }

  std::string getStreamDataPath(const char* filePath, uint32_t idx) {
    auto sdataPath = std::string(filePath).substr(0, std::string(filePath).size()-5);
    std::replace(sdataPath.begin(), sdataPath.end(), '\\', '/');
    return sdataPath + "." + std::to_string(idx) + ".sdata";
  }
}

void T3DM::writeT3DM(
  const Config &config,
  const T3DMData &t3dm,
  const std::string &t3dmPath,
  const std::vector<CustomChunk> &customChunks
)
{
  int16_t aabbMin[3] = {32767, 32767, 32767};
  int16_t aabbMax[3] = {-32768, -32768, -32768};

  uint32_t chunkIndex = 0;
  uint32_t chunkCount = 2; // vertices + indices
  if(config.createBVH)chunkCount += 1;
  chunkCount += t3dm.materials.size();
  chunkCount += customChunks.size();

  std::vector<ModelChunked> modelChunks{};
  modelChunks.reserve(t3dm.models.size());
  for(const auto & model : t3dm.models) {
    auto chunks = chunkUpModel(model);
    if(config.verbose) {
      printf("[%s] Vertices out: %ld\n", model.name.c_str(), chunks.vertices.size());
    }
    optimizeModelChunk(config, chunks);

    if(config.verbose) {
      int totalIdx=0, totalStrips=0, totalStripCmd = 0;
      for(auto &c : chunks.chunks) {
        printf("[%s:part-%ld] Vert: %d | Idx-Tris: %ld | Idx-Strip: %ld %ld %ld %ld\n",
          model.name.c_str(),
          &c - &chunks.chunks[0],
          c.vertexCount,
          c.indices.size(),
          c.stripIndices[0].size(), c.stripIndices[1].size(),
          c.stripIndices[2].size(), c.stripIndices[3].size()
        );
        totalIdx += c.indices.size();
        totalStrips += c.stripIndices[0].size() + c.stripIndices[1].size() + c.stripIndices[2].size() + c.stripIndices[3].size();
        totalStripCmd += !c.stripIndices[0].empty() + !c.stripIndices[1].empty() + !c.stripIndices[2].empty() + !c.stripIndices[3].empty();
      }
      printf("[%s] Idx-Tris: %d, Idx-Strip: %d (commands: %d)\n", model.name.c_str(), totalIdx, totalStrips, totalStripCmd);
    }

    chunks.triCount = model.triangles.size();
    modelChunks.push_back(chunks);
    chunkCount += 1; // object

    aabbMin[0] = std::min(aabbMin[0], chunks.aabbMin[0]);
    aabbMin[1] = std::min(aabbMin[1], chunks.aabbMin[1]);
    aabbMin[2] = std::min(aabbMin[2], chunks.aabbMin[2]);

    aabbMax[0] = std::max(aabbMax[0], chunks.aabbMax[0]);
    aabbMax[1] = std::max(aabbMax[1], chunks.aabbMax[1]);
    aabbMax[2] = std::max(aabbMax[2], chunks.aabbMax[2]);
  }
  chunkCount += t3dm.skeletons.empty() ? 0 : 1;
  chunkCount += t3dm.animations.size();

  std::vector<BinaryFile> streamFiles{};

  // Main file
  BinaryFile file{};
  file.writeChars("T3M", 3);
  file.write<uint8_t>(T3DM::T3DM_VERSION);
  file.write(chunkCount); // chunk count

  file.write<uint16_t>(0); // total vertex count (set later)
  file.write<uint16_t>(0); // total index count (set later)

  uint32_t offsetChunkTypeTable = file.getPos();
  file.skip(3 * sizeof(uint32_t)); // chunk type indices (filled later)

  uint32_t offsetStringTablePtr = file.getPos();
  file.skip(sizeof(uint32_t)); // string table offset (filled later)

  file.write<uint32_t>(0); // block, set by users at runtime
  file.writeArray(aabbMin, 3);
  file.writeArray(aabbMax, 3);

  uint32_t offsetChunkTable = file.getPos();
  file.skip(chunkCount * sizeof(uint32_t)); // chunk-table

  auto addToChunkTable = [&](char type) {
    uint32_t offset = file.posPush();
      file.setPos(offsetChunkTable);
      file.writeChunkPointer(type, offset);
      offsetChunkTable = file.getPos();
    file.posPop();
    ++chunkIndex;
  };

  auto addChunkTypeIndex = [&]() {
    file.posPush();
      file.setPos(offsetChunkTypeTable);
      file.write(chunkIndex);
      offsetChunkTypeTable = file.getPos();
    file.posPop();
  };

  // Chunks
  BinaryFile chunkVerts{};
  BinaryFile chunkIndices{};
  BinaryFile chunkBVH{};
  std::vector<std::shared_ptr<BinaryFile>> chunkMaterials{};
  std::vector<BinaryFile> chunkSkeletons{};

  std::string stringTable = "S";

  // now write out each model (aka. collection of mesh-parts + materials)
  int m=0;
  uint16_t totalVertCount = 0;
  uint16_t totalIndexCount = 0;

  if(!t3dm.skeletons.empty())
  {
    auto &chunkBone = chunkSkeletons.emplace_back();
    chunkBone.skip(4); // size, filed later

    int boneCount = 0;
    for(auto &skel : t3dm.skeletons) {
      boneCount += writeBone(chunkBone, skel, stringTable, config.globalScale, 0);
    }

    chunkBone.setPos(0);
    chunkBone.write<uint16_t>(boneCount);
  }

  if(config.createBVH) {
    auto bvhData = createMeshBVH(modelChunks);
    chunkBVH.writeArray(bvhData.data(), bvhData.size());
  }

  // write used materials
  std::unordered_map<std::string, uint32_t> materialMap{};
  for(auto &material_ : t3dm.materials)
  {
    auto &material = material_.second;
    uint32_t matIdx = materialMap.size();
    materialMap[material.name] = matIdx;
    auto f = std::make_shared<BinaryFile>();

    if(config.materialWriter && config.materialWriter(f, material, matIdx)) {
      chunkMaterials.push_back(f);
      continue;
    }

    f->write(material.colorCombiner);
    f->write(material.otherModeValue);
    f->write(material.otherModeMask);
    f->write(material.blendMode);
    f->write(material.drawFlags);

    f->write<uint8_t>(0);
    f->write(material.fogMode);
    f->write<uint8_t>(
      material.setPrimColor |
      (material.setEnvColor << 1) |
      (material.setBlendColor << 2)
    );
    f->write(material.vertexFxFunc);

    f->writeArray(material.primColor, 4);
    f->writeArray(material.envColor, 4);
    f->writeArray(material.blendColor, 4);
    f->write(insertString(stringTable, material.name));

    std::vector materials{&material.texA, &material.texB};
    for(const MaterialTexture* mat_ : materials) {
      const MaterialTexture&mat = *mat_;

      f->write(mat.texReference);

      if(!mat.texPathRom.empty()) {
        // check if string already exits
        auto strPos = insertString(stringTable, mat.texPathRom);

        uint32_t hash = stringHash(mat.texPathRom);
        //printf("Texture: %s (%d)\n", texPath.c_str(), hash);
        f->write((uint32_t)strPos);
        f->write(hash);

      } else {
        f->write(0);
        // if no texture is set, use the reference as hash
        // this is needed to force a reevaluation of the texture state
        f->write(mat.texReference);
      }

      f->write((uint32_t)0); // runtime pointer
      f->write((uint16_t)mat.texWidth);
      f->write((uint16_t)mat.texHeight);

      auto writeTile = [&](const T3DM::TileParam &tile) {
        f->write(tile.low);
        f->write(tile.high);
        f->write(tile.mask);
        f->write(tile.shift);
        f->write(tile.mirror);
        f->write(tile.clamp);
      };
      writeTile(mat.s);
      writeTile(mat.t);
    }

    chunkMaterials.push_back(f);
  }

  file.align(8);
  for(auto &model : t3dm.models)
  {
    addToChunkTable('O');
    uint32_t matIdx = materialMap[model.materialName];

    // write object chunk
    const auto &chunks = modelChunks[m];
    file.write(insertString(stringTable, chunks.chunks.back().name));
    file.write((uint16_t)chunks.chunks.size());
    file.write(chunks.triCount);
    file.write(matIdx);
    file.write<uint32_t>(0); // block, set at runtime
    file.write<uint32_t>(0); // visibility, set at runtime + padding
    file.writeArray(chunks.aabbMin, 3);
    file.writeArray(chunks.aabbMax, 3);

    //printf("Object %d: %d vert offset\n", m, chunkVerts.getPos());

    // Write parts, these are a collection of indices after a vertex-slice load
    for(const auto& chunk : chunks.chunks)
    {
      //printf("  t3d_vert_load(vertices, %d, %d);\n", chunk.vertexOffset, chunk.vertexCount);
      uint32_t partVertOffset = (chunk.vertexOffset * T3DM::VertexT3D::byteSize());
      partVertOffset += chunkVerts.getPos();

      file.write(partVertOffset);
      file.write<uint16_t>(chunk.vertexCount);
      file.write<uint16_t>(chunk.vertexDestOffset);
      file.write(chunkIndices.getPos());
      file.write((uint16_t)chunk.indices.size());
      file.write<uint16_t>(chunk.boneIndex); // Matrix/Bone index
      file.write((uint8_t)chunk.stripIndices[0].size());
      file.write((uint8_t)chunk.stripIndices[1].size());
      file.write((uint8_t)chunk.stripIndices[2].size());
      file.write((uint8_t)chunk.stripIndices[3].size());
      file.write(chunk.seqStart);
      file.write(chunk.seqCount);
      file.write<uint8_t>(0);
      file.write<uint8_t>(0);

      // write indices data
      chunkIndices.writeArray(chunk.indices.data(), chunk.indices.size());
      for(const auto & stripIndex : chunk.stripIndices) {
        if(stripIndex.empty())break;
        chunkIndices.align(8);
        chunkIndices.writeArray(stripIndex.data(), stripIndex.size());
      }

      totalIndexCount += chunk.indices.size();
    }

    // vertex buffer
    //printf("  Verts: %d\n", chunks.vertices.size());
    for(auto v=0; v<chunks.vertices.size(); v+=2)
    {
      const auto &vertA = chunks.vertices[v];
      const auto &vertB = chunks.vertices[v+1];

      //printf("Pos: %d %d %d | %d %d %d\n", vertA.pos[0], vertA.pos[1], vertA.pos[2], vertB.pos[0], vertB.pos[1], vertB.pos[2]);
      chunkVerts.write(vertA.pos[0]);
      chunkVerts.write(vertA.pos[1]);
      chunkVerts.write(vertA.pos[2]);
      chunkVerts.write(vertA.norm);

      chunkVerts.write(vertB.pos[0]);
      chunkVerts.write(vertB.pos[1]);
      chunkVerts.write(vertB.pos[2]);
      chunkVerts.write(vertB.norm);

      chunkVerts.write(vertA.rgba);
      chunkVerts.write(vertB.rgba);

      chunkVerts.write(vertA.s);
      chunkVerts.write(vertA.t);
      chunkVerts.write(vertB.s);
      chunkVerts.write(vertB.t);
    }
    totalVertCount += chunks.vertices.size();

    ++m;
  }

  uint16_t animIdx = 0;
  for(const auto &anim : t3dm.animations) {
    BinaryFile streamFile{};
    file.align(4);
    addToChunkTable('A');

    file.write(insertString(stringTable, anim.name));
    file.write<float>(anim.duration);
    file.write<uint32_t>(anim.keyframes.size());
    file.write<uint16_t>(anim.channelCountQuat);
    file.write<uint16_t>(anim.channelCountScalar);
    file.write<uint32_t>(insertString(stringTable,
      getRomPath(getStreamDataPath(t3dmPath.c_str(), animIdx))
    ));

    std::unordered_set<uint32_t> channelHasKF{};
    for(int k=0; k<anim.keyframes.size(); ++k) {
      bool isLastKF = (k >= anim.keyframes.size()-1);
      const auto &kf = anim.keyframes[k];
      const auto &kfNext = isLastKF ? kf : anim.keyframes[k+1];

      bool nextIsLarge = kfNext.valQuantSize > 1;

      uint16_t timeNext = kf.timeNextInChannelTicks;
      assert(timeNext < (1 << 15)); // prevent conflicts with size flag
      if(nextIsLarge)timeNext |= (1 << 15); // encode size of the next KF here

      //printf("KF[%d]: %.4f, needed: %.4f, next: %.4f\n", k, kf.time, kf.timeNeeded, kf.timeNextInChannel);

      streamFile.write<uint16_t>(timeNext);
      streamFile.write<uint16_t>(kf.chanelIdx);
      for(int v=0; v<kf.valQuantSize; ++v) {
        streamFile.write<uint16_t>(kf.valQuant[v]);
      }

      // force the first keyframe to have 2 values, this is to have a known initial state
      if(k == 0 && kf.valQuantSize == 1) {
        streamFile.write<uint16_t>(0);
      }
    }
    streamFiles.push_back(streamFile);

    for(const auto &ch : anim.channelMap) {
      file.write(ch.targetIdx);
      file.write(ch.targetType);
      file.write(ch.attributeIdx);
      file.write((ch.valueMax - ch.valueMin) / (float)0xFFFF);
      file.write(ch.valueMin);
    }

    ++animIdx;
  }

  // Now patch all chunks together and write out the chunk-table

  if(config.createBVH) {
    file.align(8);
    addToChunkTable('B');
    file.writeMemFile(chunkBVH);
  }

  file.align(16);
  addChunkTypeIndex();
  addToChunkTable('V');
  file.writeMemFile(chunkVerts);

  file.align(4);
  addChunkTypeIndex();
  addToChunkTable('I');
  file.writeMemFile(chunkIndices);

  addChunkTypeIndex();
  for(auto &f : chunkMaterials) {
    file.align(8);
    addToChunkTable(config.materialWriter ? 'm' : 'M');
    file.writeMemFile(*f);
  }

  for(const auto &chunkSkel : chunkSkeletons) {
    file.align(8);
    addToChunkTable('S');
    file.writeMemFile(chunkSkel);
  }

  for(const auto &custom : customChunks) {
    file.align(8);
    addToChunkTable(custom.type);
    file.writeArray(custom.data.data(), custom.data.size());
  }

  // String table
  file.align(4);
  uint32_t stringTableOffset = file.getPos();
  file.write(stringTable);

  file.setPos(offsetStringTablePtr);
  file.write(stringTableOffset);

  // patch vertex/index count
  file.setPos(0x08);
  file.write(totalVertCount);
  file.write(totalIndexCount);

  // write to actual file
  file.writeToFile(t3dmPath.c_str());

  for(int s=0; s<streamFiles.size(); ++s) {
    auto sdataPath = getStreamDataPath(t3dmPath.c_str(), s);
    streamFiles[s].writeToFile(sdataPath.c_str());
  }
}