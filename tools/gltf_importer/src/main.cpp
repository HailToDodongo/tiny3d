/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#include <stdio.h>
#include <string>
#include <memory>
#include <filesystem>

#include "structs.h"
#include "parser.h"

#include "binaryFile.h"
#include "converter.h"

namespace fs = std::filesystem;

namespace {
  constexpr std::size_t DEFAULT_CHUCK_SIZE = 1024*1024*4;
  constexpr float MODEL_SCALE = 64.0f;

  uint32_t stringHash(const std::string &str)
  {
    uint32_t hash = 0x7E81C0E9;
    for(char c : str) {
      hash = (hash >> 8) ^ (hash << 24) ^ c;
    }
    return hash;
  }
}

int main(int argc, char* argv[])
{
  const char* gltfPath = argv[1];
  const char* t3dmPath = argv[2];

  auto models = parseGLTF(gltfPath, MODEL_SCALE);
  fs::path gltfBasePath{gltfPath};

  uint32_t chunkIndex = 0;
  uint32_t chunkCount = 2; // vertices + indices
  std::vector<ModelChunked> modelChunks{};
  modelChunks.reserve(models.size());
  for(const auto & model : models) {
    modelChunks.push_back(chunkUpModel(model));
    chunkCount += 1 + 2; // object + material A/B (@TODO: optimize material count)
  }

  // Main file
  BinaryFile file{t3dmPath};
  file.writeChars("T3DM", 4);
  file.write(chunkCount); // chunk count

  uint32_t offsetChunkTypeTable = file.getPos();
  file.skip(3 * sizeof(uint32_t)); // chunk type indices (filled later)

  uint32_t offsetStringTablePtr = file.getPos();
  file.skip(sizeof(uint32_t)); // string table offset (filled later)

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
  BinaryFile chunkVerts{models.size() * DEFAULT_CHUCK_SIZE};
  BinaryFile chunkIndices{models.size() * DEFAULT_CHUCK_SIZE};
  std::vector<std::shared_ptr<BinaryFile>> chunkMaterials{};
  std::string stringBuffer = "S";

  // now write out each model (aka. collection of mesh-parts + materials)
  int m=0;
  uint32_t materialIndex = 0;

  file.align(8);
  for(auto &model : models)
  {
    addToChunkTable('O');

    // write material chunk(s)
    auto writeMaterial = [&chunkMaterials, &stringBuffer, &gltfBasePath](const Material &material) {
      auto f = std::make_shared<BinaryFile>(DEFAULT_CHUCK_SIZE);
      f->write(material.colorCombiner);
      f->write(material.drawFlags);

      std::string texPath = fs::relative(material.texPath, std::filesystem::current_path());

      if(texPath.find("assets/") == 0) {
        texPath.replace(0, 7, "rom:/");
      }
      if(texPath.find(".png") != std::string::npos) {
        texPath.replace(texPath.find(".png"), 4, ".sprite");
      }

      if(!texPath.empty()) {
        // check if string already exits
        auto strPos = stringBuffer.find(texPath);
        if(strPos == std::string::npos) {
          strPos = stringBuffer.size();
          stringBuffer += texPath;
          stringBuffer.push_back('\0');
        }

        uint32_t hash = stringHash(texPath);
        printf("Texture: %s (%d)\n", texPath.c_str(), hash);
        f->write((uint32_t)strPos);
        f->write(hash);

      } else {
        f->write(0);
        f->write(0);
      }

      f->write((uint32_t)0); // runtime pointer
      f->write((uint16_t)material.texWidth);
      f->write((uint16_t)material.texHeight);

      auto writeTile = [&](const TileParam &tile) {
        f->write(tile.low);
        f->write(tile.high);
        f->write(tile.mask);
        f->write(tile.shift);
        f->write(tile.mirror);
        f->write(tile.clamp);
      };

      writeTile(material.s);
      writeTile(material.t);
      chunkMaterials.push_back(f);
    };
    writeMaterial(model.materialA);
    writeMaterial(model.materialB);

    // write object chunk
    const auto &chunks = modelChunks[m];
    file.write((uint32_t)chunks.chunks.size());
    file.write(materialIndex++);
    file.write(materialIndex++);

    printf("Object %d: %d vert offset\n", m, chunkVerts.getPos());

    // Write parts, these are a collection of indices after a vertex-slice load
    for(const auto& chunk : chunks.chunks)
    {
      //printf("  t3d_vert_load(vertices, %d, %d);\n", chunk.vertexOffset, chunk.vertexCount);
      uint32_t partVertOffset = (chunk.vertexOffset * VertexT3D::byteSize());
      partVertOffset += chunkVerts.getPos();

      file.write(partVertOffset);
      file.write(chunk.vertexCount);
      file.write(chunkIndices.getPos());
      file.write((uint16_t)chunk.indices.size());
      file.write((uint8_t)0); // type
      file.write((uint8_t)0); // padding

      for(uint8_t index : chunk.indices) {
        chunkIndices.write(index);
      }
    }

    // vertex buffer
    printf("  Verts: %d\n", chunks.vertices.size());
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

    ++m;
  }

  // Now patch all chunks together and write out the chunk-table

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
    addToChunkTable('M');
    file.writeMemFile(*f);
  }

  // String table
  file.align(4);
  uint32_t stringTableOffset = file.getPos();
  file.write(stringBuffer);

  file.setPos(offsetStringTablePtr);
  file.write(stringTableOffset);
}