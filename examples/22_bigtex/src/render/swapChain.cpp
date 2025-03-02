/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "swapChain.h"
#include "../utils/fifo.h"
#include "vi.h"

namespace {
  constexpr uint32_t FB_COUNT = 3;
  volatile uint8_t fbIdxVI = 0;

  std::array<uint8_t, FB_COUNT> fbState{}; // current render-pass index
  FIFO<uint8_t, 0xFF, FB_COUNT> fbIdxForVI{};
  volatile uint32_t fbFreeCount = 0; // amount of 'fbState' at zero, used for a faster loop

  // prevent a new frame from being started, this is done to avoid multiple passes in parallel.
  // At least up until the VI takes over. Otherwise, it runs risk of doing 2 RDP passes in parallel
  // leading to random corruptions
  volatile uint8_t blockNewFrame = false;
  surface_t *frameBuffers = nullptr;

  SwapChain::RenderPassDrawTask drawTask{nullptr};
  uint32_t frameSkip = 0;
  uint32_t frameIdx = 0;

  void on_vi_frame_ready()
  {
    if(++frameIdx <= frameSkip)return;
    disable_interrupts();
    auto nextFbIdx = fbIdxForVI.pop();

    if(nextFbIdx != 0xFF) {
      vi_write_begin();
      vi_show(&frameBuffers[nextFbIdx]);
      vi_write_end();

      ++fbState[nextFbIdx];
      fbState[fbIdxVI] = 0;
      fbFreeCount += 1;
      fbIdxVI = nextFbIdx;
    }
    enable_interrupts();
    frameIdx = 0;
  }

  /**
   * Gets called by an async render-pass, this will mark it as ready for the next pass.
   * This is usually called from within an interrupt, so we have to defer the transition to the next pass.
   * @param fbIndex
   */
  void renderPassDone(uint32_t fbIndex)
  {
    disable_interrupts();
    ++fbState[fbIndex];
    fbIdxForVI.push(fbIndex);
    blockNewFrame = false;
    enable_interrupts();
  }
}

void SwapChain::init(const SwapChainConf &conf)
{
  frameBuffers = conf.frameBuffers;
  blockNewFrame = false;

  fbState.fill({0xFF-1}); // block all buffers...
  fbState[1] = 0; // ...except second, picked up by first render-pass
  fbFreeCount = 1;
  // to get started, pretend the VI already has 1 frame rendering
  // this will start the logic of VI chasing finished buffers + freeing drawn ones
  fbIdxVI = FB_COUNT-1;
  fbIdxForVI.fill(0xFF); // clear FIFO...
  fbIdxForVI.push(0); // ...and make VI render the first buffer

  disable_interrupts();
    register_VI_handler(on_vi_frame_ready);
    set_VI_interrupt(1, VI_V_CURRENT_VBLANK);
  enable_interrupts();

  rspq_wait();
}

void SwapChain::nextFrame() {
  for (uint32_t __t = TICKS_READ() + TICKS_FROM_MS(200);; __rsp_check_assert(__FILE__, __LINE__, __func__))
  {
    if(fbFreeCount && !blockNewFrame)break;
    if(!TICKS_BEFORE(TICKS_READ(), __t)) {
      //rsp_crashf("wait loop timed out (%d ms)", 200);
      debugf("[ERROR] RSP wait loop timed, force new buffer\n");
      fbFreeCount = 1;
      blockNewFrame = false;
    }
  }

  uint32_t freeIdx = 0;
  while(fbState[freeIdx])++freeIdx;

  disable_interrupts();
  fbFreeCount -= 1;
  blockNewFrame = true;
  enable_interrupts();

  drawTask(&frameBuffers[freeIdx], freeIdx, renderPassDone);
}

void SwapChain::drain() {
  rspq_wait();
  RSP_WAIT_LOOP(200) {
    // if only one buffer is not free (must be VI), we are done
    if(fbFreeCount == (FB_COUNT - 1))break;
  }
  blockNewFrame = false;
}
void SwapChain::setFrameSkip(uint32_t skip) {
  frameSkip = skip;
}

void SwapChain::setDrawPass(SwapChain::RenderPassDrawTask task) {
  drawTask = task;
}

void SwapChain::start() {
  vi_write_begin();
    vi_show(&frameBuffers[fbIdxVI]);
  vi_write_end();
  wait_ms(30);
}

surface_t *SwapChain::getFrameBuffer(uint32_t idx) {
  return &frameBuffers[idx];
}
