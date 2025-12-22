/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "pointGlobe.h"
#include "../scene/scene.h"
#include "../main.h"

namespace {
  constexpr float BASE_SCALE = 0.8f;

  constexpr int SLICE_SIZE = 500;
  constexpr float SLICE_SPEED = 1000.0f;
  constexpr float SLICE_COOLDOWN = 6.0f;

  constexpr float FADE_DIST_START = 400.0f;
  constexpr float FADE_DIST_END = 300.0f;

  constexpr uint8_t lightPointColor[4] = {0xFF, 0x77, 0, 0};
}

// Shared resources:
namespace {
  sprite_t *ptTex{};
  rspq_block_t *dplMat{};

  uint32_t refCount{0};

  constexpr uint8_t sampleMap(uint8_t* data, float x, float y) {
    uint32_t coordX = x * 256;
    uint32_t coordY = y * 128;
    coordX %= 256;
    coordY %= 128;
    return data[coordY * 256 + coordX];
  }

  constexpr color_t sampleMapColor(color_t* data, float x, float y) {
    uint32_t coordX = x * 256;
    uint32_t coordY = y * 128;
    coordX %= 256;
    coordY %= 128;
    return data[coordY * 256 + coordX];
  }
}

namespace Actor
{
  PointGlobe::PointGlobe(const fm_vec3_t &_pos, const Args &_args)
  {
    if(refCount++ == 0) {
      ptTex = sprite_load("rom:/spheres.i8.sprite");

      rspq_block_begin();
        rdpq_sync_pipe();
        rdpq_sync_tile();

        rdpq_mode_combiner(RDPQ_COMBINER1((PRIM,0,TEX0,0),    (0,0,0,TEX0)));
        rdpq_mode_zbuf(true, true);

        rdpq_texparms_t params{};
        params.s.repeats = REPEAT_INFINITE;
        params.t.repeats = REPEAT_INFINITE;
        //params.s.mirror = true;
        params.s.translate = 0.5f;
        params.t.translate = 0.5f;
        params.s.scale_log = -3;
        params.t.scale_log = -3;
        rdpq_sprite_upload(TILE0, ptTex, &params);

        tpx_state_set_tex_params(0, 0);
        tpx_state_set_scale(0.5f, 0.5f);
      dplMat = rspq_block_end();
    }

    sprite_t *texWorld = sprite_load("rom:/worldMap.rgba32.sprite");
    auto worldMap = (color_t*)sprite_get_pixels(texWorld).buffer;

    pos = _pos;
    args = _args;
    timer = (rand() % 100) / 25;
    args.scale *= BASE_SCALE;

    uint32_t sampleCount = 2000;
    particles.resize(sampleCount);
    particles.count = 0;

    float phi = T3D_PI * (sqrtf(5.0f) - 1.0f);  // golden angle in radians

    float latAngle = 0.0f;
    float lonAngle = T3D_PI / 2.0f;

    float longIncr = (T3D_PI) / sampleCount;
    float latIncr = (T3D_PI * 32.0f) / sampleCount;

    for(uint32_t i=0; i<sampleCount; ++i) {
      auto p = tpx_buffer_get_pos(particles.particles, particles.count);
      auto col = tpx_buffer_get_rgba(particles.particles, particles.count);

      float y = 1.0f - (i / (float)(sampleCount - 1)) * 2.0f;//  # y goes from 1 to -1
      float radius = sqrtf(1.0f - y * y);
      float theta = phi * i;

      color_t colImg = sampleMapColor(worldMap, theta * (0.5f / T3D_PI), -y * 0.5f + 0.5f);
      uint8_t brightness = colImg.a;
      if(brightness < 20)continue;

      if(particles.count >= particles.countMax)break;
      ++particles.count;

      float displ = colImg.r / 255.0f;
      displ = displ * 0.2f + 0.8f;

      auto pt = fm_vec3_t{
        fm_cosf(theta) * radius,
        y,
        fm_sinf(theta) * radius
      } * (displ * 127.0f);

      p[0] = pt.x;
      p[1] = pt.y;
      p[2] = pt.z;

      *tpx_buffer_get_size(particles.particles, particles.count) = 35 + (rand()%5);

      col[0] = colImg.r;
      col[1] = colImg.g;
      col[2] = colImg.b;
      col[3] = 0;

      lonAngle += longIncr;
      latAngle += latIncr;
    }

    sprite_free(texWorld);
  }

  PointGlobe::~PointGlobe()
  {
    if(--refCount == 0) {
      sprite_free(ptTex);
      ptTex = nullptr;
      rspq_block_free(dplMat);
      dplMat = nullptr;
    }
  }

  void PointGlobe::update(float deltaTime)
  {
    float camDist = t3d_vec3_distance(
      &state.activeScene->getCam().pos,
      &pos
    );

    if(camDist > FADE_DIST_START) {
      actualScale = 0;
      t3d_light_set_directional(0, color_t{0,0,0,0}, pos); // disable light
      return;
    }

    float fade;
    if(camDist < FADE_DIST_END) {
      fade = 1.0f;
    } else {
      fade = (FADE_DIST_START - camDist) / (FADE_DIST_START - FADE_DIST_END);
      fade = fmaxf(fade, 0.0f);
    }

    timerFlicker += deltaTime;
    if(timerFlicker > 100.0f)timerFlicker = 0;

    float visibleFactor = ((float)lastDrawnPartCount / (float)particles.countMax);
    visibleFactor = fmaxf(visibleFactor, 0.7f);
    float lightStrength = fade * visibleFactor * 200.5f;
    lightStrength += fm_sinf(timerFlicker*15.0f) * 4.0f;
    t3d_light_set_point(0, lightPointColor, &pos, lightStrength, false);

    timer += deltaTime * 0.25f;
    timerNoise += deltaTime;
    actualScale = fm_sinf(timer) * 0.5f + 0.5f;
    actualScale = fminf(actualScale + 0.75f, 1.0f) * args.scale;
    actualScale *= fade;

    t3d_mat4fp_from_srt_euler(particles.mat,
      {actualScale, actualScale, actualScale},
      {0, timer*1.2f, 0.25f},
      pos
    );
  }

  void PointGlobe::drawPTX(float deltaTime)
  {
    if(actualScale <= 0)return;
    rspq_block_run(dplMat);
    lastDrawnPartCount = particles.count;

    if(timerNoise > 0.0f) {
      lastDrawnPartCount = 0;

      int noiseSection = timerNoise * SLICE_SPEED;
      noiseSection -= SLICE_SIZE;
      lastDrawnPartCount += particles.drawTexturedSlice(0, noiseSection - SLICE_SIZE);
      lastDrawnPartCount += particles.drawTexturedSlice(noiseSection + SLICE_SIZE, particles.countMax);
      if((noiseSection-SLICE_SIZE) > (int)particles.countMax) {
        timerNoise = -SLICE_COOLDOWN;
      }
    } else {
      particles.drawTextured();
    }
  }
}