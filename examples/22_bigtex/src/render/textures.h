/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once

#include <libdragon.h>
#include <string>
#include <unordered_map>

class Textures
{
  private:
    uint8_t* buffer{};
    uint32_t maxSize{};
    uint32_t idx{0};

    std::unordered_map<std::string, uint8_t> texMap{};

  public:
    Textures(uint32_t maxSize);
    ~Textures();

    uint8_t addTexture(const std::string &path);
    uint8_t reserveTexture();

    uint8_t setTexture(uint8_t idx, const std::string &pathNew);

    [[nodiscard]] const uint8_t* getBuffer() const { return buffer; }
};