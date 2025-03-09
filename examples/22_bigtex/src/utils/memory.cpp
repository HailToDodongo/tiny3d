/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "memory.h"
#include "../main.h"

extern "C" {
  // "libdragon/system_internal.h"
  void* sbrk_top(int incr);
}

namespace {
  heap_stats_t heap_stats{};
  surface_t zBuffer[2]{};

  constexpr uint32_t FB_BYTE_SIZE = 320*240*2;
  constexpr uint32_t FB_BANK_ADDR[6] = {
    0x80500000 - FB_BYTE_SIZE, 0x80500000,
    0x80600000 - FB_BYTE_SIZE, 0x80600000,
    0x80700000 - FB_BYTE_SIZE, 0x80700000,
  };
}

void Memory::dumpHeap(const char *name) {
  auto oldUsed = heap_stats.used;
  sys_get_heap_stats(&heap_stats);
  if(name) {
    debugf("[%s]: Heap: %d | diff: %d\n", name, heap_stats.used, heap_stats.used - oldUsed);
  } else {
    debugf("Heap: %d\n", heap_stats.used);
  }
}

surface_t *Memory::getZBuffer(uint32_t idx) {
  assert(zBuffer[idx].buffer);
  return &zBuffer[idx];
}

Memory::FrameBuffers Memory::allocOptimalFrameBuffers() {
  if(is_memory_expanded()) { // With 8MB, we reserve the upper 4MB (excl. the stack) for the frame-buffers
    // first limit the upper heap to match against the start of our first buffer
    void *buf = sbrk_top(16); // probe current end
    uint32_t missing = (uint32_t)buf - FB_BANK_ADDR[0]; // reserve rest
    buf = sbrk_top(missing);
    assert(FB_BANK_ADDR[0] == (uint32_t)buf);

    zBuffer[0] = surface_make(UncachedAddr(FB_BANK_ADDR[5]), FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*2);
    zBuffer[1] = surface_make(UncachedAddr(FB_BANK_ADDR[5] + (320*240*2)), FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*2);

    return {
      .color = {
        surface_make(UncachedAddr(FB_BANK_ADDR[2]), FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*2),
        surface_make(UncachedAddr(FB_BANK_ADDR[3]), FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*2),
        surface_make(UncachedAddr(FB_BANK_ADDR[4]), FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*2),
        //surface_alloc(FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT),
        //surface_alloc(FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT),
        //surface_alloc(FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT),
      },
      .uv = {
        surface_alloc(FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT),
        surface_alloc(FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT),
        surface_alloc(FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT),
      },
      .shade = {
        surface_alloc(FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT),
        surface_alloc(FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT),
        surface_alloc(FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT),
        //surface_make(UncachedAddr(FB_BANK_ADDR[0]), FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*2),
        //surface_make(UncachedAddr(FB_BANK_ADDR[1]), FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*2),
        //surface_make(UncachedAddr(FB_BANK_ADDR[1]), FMT_RGBA16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*2),

      },
      .depth = {&zBuffer[0], &zBuffer[1]}
    };
  } else {
    assertf(false, "Expansion-Pack required!");
  }
}

