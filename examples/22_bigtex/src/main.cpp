/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <t3d/t3d.h>
#include <string>

#include "render/textures.h"
#include "render/hdModel.h"
#include "utils/memory.h"
#include "render/swapChain.h"
#include "render/debugDraw.h"
#include "rsp/rspFX.h"
#include "render/skybox.h"
#include "main.h"
#include "scene/scene.h"

/**
 * NOTE: this is a more complex project not showing any direct t3d API, but rather a small project making use of it.
 * If you want to learn t3d, checkout the other examples first.
 *
 * Example using high-res 256x256px RGBA16 textures.
 * This is a more advanced technique that performes deferred rendering for shading and texturing.
 * Instead of letting the RDP texture triangles directly, it instead writes UVs into an off-screen buffer.
 * Later the CPU will pick this up and performs the texel-lookup.
 * Shading (if enabled) is in yet another off-screen buffer, which is finally combines with the RDP.
 * While this may seem straight-forward, there is a lot of logic and micro-optimizations necessary
 * to make this run at a decent speed.
 * For example, raw assembly for the CPU taks, ucode, and a custom compression format for textures.
 * With just the skybox visible i can mostly reach 60FPS by now.
 *
 * For more details, checkout the comments in the individual functions.
 */

State state{};

[[noreturn]]
int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  rdpq_init();
  //rdpq_debug_start();

  joypad_init();
  t3d_init((T3DInitParams){});
  RSP::FX::init();

  uint64_t lastTime = get_ticks() - 10000;
  float lastDeltas[32] = {0};

  auto buffers = Memory::allocOptimalFrameBuffers();
  Debug::init();

  vi_init();
  vi_set_dedither(false);
  vi_set_aa_mode(vi_aa_mode_t::VI_AA_MODE_RESAMPLE);
  vi_set_interlaced(false);
  vi_set_divot(false);
  vi_set_gamma(vi_gamma_t::VI_GAMMA_DISABLE);

  SwapChain::init({
    .frameBuffers = buffers.color,
    .frameBufferCount = 3
  });

  for(;;)
  {
    Memory::dumpHeap("Before scene");
    auto ticksScene = get_ticks();

    Scene scene{};

    ticksScene = get_ticks() - ticksScene;
    debugf("Scene load: %lldms\n", TICKS_TO_MS(get_ticks() - ticksScene));

    SwapChain::setDrawPass([&scene, &buffers](surface_t *surf, uint32_t fbIndex, auto done) {
      rdpq_attach(surf, Memory::getZBuffer());
      scene.draw(buffers, surf);
      rdpq_detach_cb((void(*)(void*))(done), (void*)fbIndex);
    });
    SwapChain::start();

    int lastMapModel = state.mapModel;
    while(state.mapModel == lastMapModel)
    {
      auto ticksDelta = get_ticks() - lastTime;
      lastTime = get_ticks();
      lastDeltas[state.frame % 32] = (TICKS_TO_US(ticksDelta) / 1000.0f);

      state.fps = 0.0f;
      for(auto &f : lastDeltas)state.fps += f / 32.0f;
      float deltaTime = state.fps / 1000.0f;
      state.fps = roundf(10000.0f / state.fps) / 10.0f;
      state.frameIdx = state.frame % 3;

      if (state.limitFPS) {
        SwapChain::setFrameSkip(1);
        deltaTime = 1.0f / 30.0f;
      } else {
        SwapChain::setFrameSkip(0);
      }

      joypad_poll();
      scene.update(deltaTime);

      SwapChain::nextFrame();
      ++state.frame;
    }

    SwapChain::drain();
    debugf("Switching map model\n");
  }
}
