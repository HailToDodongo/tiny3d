/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "matrixManager.h"
#include "../main.h"

namespace {
  constexpr uint32_t MATRIX_COUNT = 64 * 3;

  constexpr uint32_t FLAG_BITS = sizeof(uint32_t)*8;
  static_assert(MATRIX_COUNT % FLAG_BITS == 0);

  uint32_t usedFlags[MATRIX_COUNT / FLAG_BITS]{};
  uint32_t laseIndex = 0;
  T3DMat4FP *bufferPtr{nullptr};

  T3DMat4FP buffer[MATRIX_COUNT]{}; // mask of used matrices, each bit represents one matrix
}

void MatrixManager::reset() {
  data_cache_hit_writeback_invalidate(buffer, MATRIX_COUNT * sizeof(T3DMat4FP));
  bufferPtr = (T3DMat4FP*)UncachedAddr(buffer);
  laseIndex = 0;
  memset(usedFlags, 0, sizeof(usedFlags));
  debugf("MatrixManager: count=%ld size=%ldkb\n", MATRIX_COUNT, sizeof(T3DMat4FP) * MATRIX_COUNT / 1024);
}

T3DMat4FP *MatrixManager::alloc(uint32_t count) {
  uint32_t countMask = (1 << count) - 1;
  for(uint32_t i=0; i<MATRIX_COUNT/FLAG_BITS; ++i) {
    uint32_t idx = (laseIndex + i) % (MATRIX_COUNT / FLAG_BITS);

    // we only consider contiguous blocks of free matrices
    for(uint32_t b=0; b<FLAG_BITS-count; ++b)
    {
      uint32_t mask = countMask << b;
      if((usedFlags[idx] & mask) == 0) {
        usedFlags[idx] |= mask;
        laseIndex = idx;
        return bufferPtr + (idx * FLAG_BITS + b);
      }
    }
  }

  debugf("##### MatrixManager: Out of matrices!\n");
  return nullptr;
}

void MatrixManager::free(T3DMat4FP *mat, uint32_t count) {
  uint32_t idx = (mat - bufferPtr) / FLAG_BITS;
  uint32_t countMask = (1 << count) - 1;
  uint32_t b = (mat - bufferPtr) % FLAG_BITS;
  usedFlags[idx] &= ~(countMask << b);
}

bool MatrixManager::isUsed(uint32_t index) {
  return usedFlags[index / FLAG_BITS] & (1 << (index % FLAG_BITS));
}

uint32_t MatrixManager::getTotalCapacity() {
  return MATRIX_COUNT;
}

