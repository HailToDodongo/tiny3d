/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#pragma once

#include <cstdio>
#include <unordered_map>
#include "types.h"
#include "byteswap.h"

class BinaryFile
{
  private:
    FILE* fp{nullptr};
    std::unordered_map<std::string, uint32_t> patchMap{};
    std::vector<uint32_t> posStack{};
    std::vector<uint8_t> data{};

  public:
    explicit BinaryFile(const char* path) {
      fp = fopen(path, "wb");
    }

    explicit BinaryFile(std::size_t size) {
      data.resize(size);
      fp = fmemopen(data.data(), size, "wb");
    }

    ~BinaryFile() { close(); }

    void close() {
      if(fp)fclose(fp);
      fp = nullptr;
    }

    void skip(u32 bytes) {
      for(u32 i=0; i<bytes; ++i) {
        write<u8>(0);
      }
    }

    template<typename T>
    void write(T value) {
      if constexpr (std::is_same_v<T, float>) {
        uint32_t val = Bit::byteswap(std::bit_cast<uint32_t>(value));
        fwrite(&val, sizeof(T), 1, fp);
      } else {
        auto val = Bit::byteswap(value);
        fwrite(&val, sizeof(T), 1, fp);
      }
    }

    void write(const std::string &str) {
      fwrite(str.data(), 1, str.size(), fp);
    }

    void writeChars(const char* str, size_t len) {
      fwrite(str, 1, len, fp);
    }

    void writeMemFile(BinaryFile& memFile) {
      fflush(memFile.fp);
      fwrite(memFile.data.data(), 1, memFile.getPos(), fp);
    }

    void writePlaceholder(const std::string& name) {
      patchMap[name] = ftell(fp);
      write<u32>(0);
    }

    void patchPlaceholder(const std::string& name, uint32_t value) {
      auto it = patchMap.find(name);
      if(it == patchMap.end()) {
        printf("Error: Patch placeholder '%s' not found!\n", name.c_str());
        return;
      }
      u32 pos = ftell(fp);
      fseek(fp, it->second, SEEK_SET);
      write<u32>(value);
      fseek(fp, pos, SEEK_SET);

      patchMap.erase(it);
    }

    void writeChunkPointer(char type, uint32_t offset) {
      offset = offset & 0xFF'FFFF;
      offset |= (uint32_t)type << 24;
      write(offset);
    }

    uint32_t getPos() {
      return ftell(fp);
    }

    void setPos(u32 pos) {
      fseek(fp, pos, SEEK_SET);
    }

    uint32_t posPush() {
      uint32_t oldPos = getPos();
      posStack.push_back(oldPos);
      return oldPos;
    }

    uint32_t posPop() {
      uint32_t oldPos = getPos();
      fseek(fp, posStack.back(), SEEK_SET);
      posStack.pop_back();
      return oldPos;
    }

    void align(u32 alignment) {
      u32 pos = getPos();
      u32 offset = pos % alignment;
      if(offset != 0) {
        u32 padding = alignment - offset;
        for(u32 i=0; i<padding; ++i) {
          write<u8>(0);
        }
      }
    }

};