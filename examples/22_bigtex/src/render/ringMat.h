/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <t3d/t3d.h>

struct RingMat4FP {
  constexpr static uint32_t COUNT = 3;

  T3DMat4FP *mat{};
  uint32_t idx{0};
  inline RingMat4FP() {
     mat = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP) * COUNT);
    //mat = MatrixManager::alloc(COUNT);
  }
  inline ~RingMat4FP() {
    free_uncached(mat);
    //MatrixManager::free(mat, COUNT);
  }

  void next() { idx = (idx + 1) % 3; }
  [[nodiscard]] T3DMat4FP* get() const { return mat + idx; }

  [[nodiscard]] T3DMat4FP* getNext() {
    next();
    return get();
  }

  void fillSRT(const T3DVec3 &scale, const T3DVec3 &rot, const T3DVec3 &trans) {
    for(uint32_t i=0; i<COUNT; ++i) {
      t3d_mat4fp_from_srt_euler(getNext(), scale, rot, trans);
    }
  }
};