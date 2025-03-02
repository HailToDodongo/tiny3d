/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

template<typename T, T DEF, uint32_t SIZE>
struct FIFO
{
  T data[SIZE]{};
  uint32_t pos{0};
  uint32_t count{0};

  void fill(const T &val) {
    for(auto &v : data)v = val;
  }

  void push(const T &val) {
    data[(pos + count) % SIZE] = val;
    if(count < SIZE)count++;
    else pos = (pos + 1) % SIZE;
  }

  T pop() {
    if(count == 0)return DEF;
    auto val = data[pos];
    pos = (pos + 1) % SIZE;
    count--;
    return val;
  }
};