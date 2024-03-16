/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once

#include <string>

inline uint32_t stringHash(const std::string &str)
{
  uint32_t hash = 0x7E81C0E9;
  for(char c : str) {
    hash = (hash >> 8) ^ (hash << 24) ^ c;
  }
  return hash;
}