/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "scene.h"
#include "../main.h"
#include "../utils/memory.h"
#include "../rsp/rspFX.h"
#include "../render/fbCpuDraw.h"
#include "../render/debugDraw.h"
#include "debugMenu.h"

extern "C" {
  // see: ./tex.S
  extern uint32_t applyTexture(uint32_t fbTexIn, uint32_t fbTexInEnd, uint32_t fbOut64);

  const char* const MAP_PATHS[3] = {
    "rom:/model.t3dm",
    "rom:/model2.t3dm",
    "rom:/model3.t3dm",
  };
}

Scene::Scene()
  : mapModel{MAP_PATHS[state.mapModel], textures}
{
  skybox.change(state.currSkybox);
}

void Scene::update(float deltaTime) {
  auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
  if(btn.start)state.menuOpen = !state.menuOpen;

  camera.setCamLerpSpeed(state.slowCam ? 0.5f : 0);
  camera.update(deltaTime);
  skybox.update(camera.getPosition());
  lightTimer += deltaTime;
}

void Scene::draw(const Memory::FrameBuffers &buffers, surface_t *surf)
{
  surface_t *surfDepth = Memory::getZBuffer();
  uint32_t frameIdxLast = (state.frameIdx + 2) % 3;

  t3d_frame_start();
  rdpq_mode_antialias(AA_NONE);
  rdpq_mode_dithering(DITHER_NONE_NONE);

  uvTex.use();
  camera.attach();

  rdpq_set_color_image(surfDepth);

  rdpq_mode_push();
    rdpq_set_mode_fill(color_from_packed16(0xFFFE));
    rdpq_fill_rectangle(0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
  rdpq_mode_pop();


  T3DVec3 lightDir{
    fm_sinf(lightTimer*0.8f) * 0.75f,
    0.75f,
    fm_cosf(lightTimer*0.8f) * 0.75f
  };
  t3d_vec3_norm(lightDir);

  switch(state.currSkybox) {
    case 0: default:
      t3d_light_set_ambient({0x99, 0x99, 0x99, 0x99});
      t3d_light_set_directional(0, {0xFF, 0x99, 0x99, 0xFF}, lightDir);
    break;
    case 1:
      t3d_light_set_ambient({0xFF, 0xFF, 0xFF, 0x99});
      t3d_light_set_directional(0, {0xFF, 0xFF, 0xFF, 0xFF}, lightDir);
    break;
    case 2:
      t3d_light_set_ambient({0x44, 0x44, 0x44, 0x99});
      t3d_light_set_directional(0, {168, 160, 44, 0xFF}, lightDir);
    break;
    case 3:
      t3d_light_set_ambient({0x11, 0x11, 0x11, 0x99});
      t3d_light_set_directional(0, {255, 255, 255, 0xFF}, lightDir);
    break;
  }
  t3d_light_set_count(0);

  rdpq_set_color_image(&buffers.uv[state.frameIdx]);
  rdpq_set_z_image(surfDepth);

  t3d_matrix_push_pos(1);

  // First draw, this will write the UVs into the offscreen-buffer.
  // Note that this will draw the *next* frame, so that the CPU task that follows doesn't have to wait for it.
  skybox.draw();
  if(state.drawMap)mapModel.draw();

  rdpq_sync_pipe();
  rdpq_mode_zbuf(false, false);

  // Second draw, if shading is enabled, this will draw the lighting + vertex colors into a different buffer
  if(state.drawShade && state.drawMap) {
    rdpq_set_color_image(&buffers.shade[state.frameIdx]);
    rdpq_mode_push();
      rdpq_set_mode_fill({0});
      rdpq_fill_rectangle(0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
    rdpq_mode_pop();
    mapModel.drawShade();
    rdpq_sync_pipe();
  }

  t3d_matrix_pop(1);

  rdpq_sync_tile();
  rdpq_sync_load();
  rdpq_set_color_image(surf);
  rdpq_set_mode_standard();

  rdpq_set_lookup_address(1, buffers.shade[frameIdxLast].buffer);
  rdpq_set_lookup_address(2, surf->buffer);

  rspq_flush();

  uint64_t ticks = get_ticks();
  uint32_t FB_SIZE_IN = SCREEN_WIDTH * SCREEN_HEIGHT * 4;
  auto *texIn = (uint64_t*)CachedAddr(buffers.uv[frameIdxLast].buffer);

  switch(state.drawMode)
  {
    case State::DRAW_MODE_DEF: {
      uint32_t quarterSlice = FB_SIZE_IN / SHADE_BLEND_SLICES / 3;
      uint32_t stepSizeTexIn = quarterSlice * 2;
      uint32_t stepSizeTexInRSP = quarterSlice * 1;

      uint32_t ptrInPos = (uint32_t)(texIn);
      uint32_t ptrOutPos = (uint32_t) CachedAddr(surf->buffer);

      // Texture look-up
      // This starts the CPU/RSP tasks to convert UVs into actual pixels.
      // RSP and CPU are doing this in slices, so that they can work in parallel...
      if(state.drawShade && state.drawMap)
      {
        for(int p=0; p<SHADE_BLEND_SLICES; ++p) {
          if(p % 4 == 0)
          {
            RSP::FX::fillTextures(ptrInPos, ptrInPos + stepSizeTexInRSP, ptrOutPos);
            rspq_flush(); // <- make sure to flush to guarantee that the RSP is busy
            ptrInPos += stepSizeTexInRSP;
            ptrOutPos += stepSizeTexInRSP / 2;
            applyTexture(ptrInPos, ptrInPos + stepSizeTexIn, ptrOutPos);
            ptrInPos += stepSizeTexIn;
            ptrOutPos += stepSizeTexIn / 2;
          } else {
            applyTexture(ptrInPos, ptrInPos + quarterSlice*3, ptrOutPos);
            ptrInPos += quarterSlice * 3;
            ptrOutPos += quarterSlice * 3 / 2;
          }
          data_cache_hit_writeback_invalidate((char*)CachedAddr(ptrOutPos) - 0x1000, 0x1000);
          // ...in the case of shading enabled, we also start the final blend inbetween,
          // this can once again run in parallel
          fbBlend.blend(p);

          rspq_flush();
        }
      } else {
        // version without shading, this is the same as above without the RDP tasks inbetween
        stepSizeTexIn =  FB_SIZE_IN / SHADE_BLEND_SLICES;
        for(int p=0; p<SHADE_BLEND_SLICES; ++p)
        {
          if(p % 4 == 0) {
            RSP::FX::fillTextures(
              ptrInPos, ptrInPos + stepSizeTexIn, ptrOutPos
            );
            rspq_flush();
          } else {
            applyTexture(ptrInPos, ptrInPos + stepSizeTexIn, ptrOutPos);
            data_cache_hit_writeback_invalidate((char*)CachedAddr(ptrOutPos + stepSizeTexIn/2) - 0x1000, 0x1000);
          }

          ptrOutPos += stepSizeTexIn / 2;
          ptrInPos += stepSizeTexIn;
        }
      }
    } break;
    case State::DRAW_MODE_UV:  FbCPU::applyTexturesUV(texIn, (uint16_t*)surf->buffer, FB_SIZE_IN); break;
    case State::DRAW_MODE_MAT: FbCPU::applyTexturesMat(texIn, (uint16_t*)surf->buffer, FB_SIZE_IN); break;
  }
  ticks = get_ticks() - ticks;

  Debug::printStart();
  if(state.menuOpen) {
    DebugMenu::draw(skybox);
  }

  switch(state.drawMode)
  {
    case State::DRAW_MODE_DEF: Debug::printf(SCREEN_WIDTH - 46, 18, state.limitFPS ? "%.0f#" : "%.1f", state.fps); break;
    case State::DRAW_MODE_UV : Debug::print(SCREEN_WIDTH - 40, 18, "UV"); break;
    case State::DRAW_MODE_MAT: Debug::print(SCREEN_WIDTH - 80, 18, "Material"); break;
  }

  if((state.frame % 60 == 0) && state.menuOpen) {
    float rec = (TICKS_TO_US(ticks) / 1000.0f);
    if(rec < 0.001f)rec = 0.001f;
    float maxFPS = 1000.0f / rec;

    debugf("Time: %lld | FPS: %.2f (max: %.2f)\n", TICKS_TO_US(ticks), state.fps, maxFPS);
  }
}

