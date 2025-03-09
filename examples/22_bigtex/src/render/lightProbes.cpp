/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "lightProbes.h"
#include "debugDraw.h"

namespace {
  constexpr float SCALE_FACTOR = 32.0f;
}

LightProbes::LightProbes(const std::string &path) {
  auto f = asset_fopen(path.c_str(), nullptr);
  uint32_t probeCount;
  fread(&probeCount, 1, 4, f);

  probes.reserve(probeCount);
  for(uint32_t i=0; i<probeCount; ++i) {
    LightProbe p{};
    fread(&p.pos, 1, 6, f);
    for(int c=0; c<6; ++c) {
      fread(&p.colors[c], 1, 3, f);
    }

    probes.push_back(p);
  }
}

LightProbeRes LightProbes::query(const T3DVec3 &pos) const {

  constexpr float POS_SCALE_FACTOR = 1.0f / 0.15f;
  int16_t posInt[3] = {
    static_cast<int16_t>(pos.x * POS_SCALE_FACTOR),
    static_cast<int16_t>(pos.y * POS_SCALE_FACTOR),
    static_cast<int16_t>(pos.z * POS_SCALE_FACTOR)
  };

  constexpr int BLEND_COUNT = 5;
  int32_t minDist[BLEND_COUNT];
  const LightProbe* minProbes[BLEND_COUNT]{};
  for(int32_t &i : minDist)i = INT32_MAX;

  for(const auto &probe : probes) {
    int32_t dist = 0;

    //Debug::drawAABB(T3DVec3{(float)probe.pos[0], (float)probe.pos[1], (float)probe.pos[2]} * 0.15f, {1,1,1}, probe.color);
    //debugf("Probe: %d %d %d | Pos: %d %d %d\n", probe.pos[0], probe.pos[1], probe.pos[2], posInt[0], posInt[1], posInt[2]);

    for(int i=0; i<3; ++i) {
      int32_t d = probe.pos[i] - posInt[i];
      dist += d*d;
    }
    if(dist > 80'000)continue;

    for(int i=0; i<BLEND_COUNT; ++i) {
      if(dist < minDist[i]) {
        for(int j=BLEND_COUNT-1; j>i; --j) {
          minDist[j] = minDist[j-1];
          minProbes[j] = minProbes[j-1];
        }
        minDist[i] = dist;
        minProbes[i] = &probe;
        break;
      }
    }
  }

  float distSum = 0.0f;
  float distFloat[BLEND_COUNT];
  for(int i=0; i<BLEND_COUNT; ++i) {
    if(minProbes[i]) {
      distFloat[i] = 100.0f / sqrtf((float)(minDist[i]));
      distSum += distFloat[i];
    }
  }

  float colorsFloat[6][3]{};

  for(int i=0; i<BLEND_COUNT; ++i) {
    if(minProbes[i]) {
      auto &probe = *minProbes[i];
      float weight = distFloat[i] / distSum;
      //debugf("Weight[%d]: %f (%f | %f)\n", i, weight, distFloat[i], distSum);
      for(int c=0; c<6; ++c) {
        colorsFloat[c][0] += (float)probe.colors[c][0] * weight;
        colorsFloat[c][1] += (float)probe.colors[c][1] * weight;
        colorsFloat[c][2] += (float)probe.colors[c][2] * weight;
      }
    }
  }

  LightProbeRes res;
  for(int c=0; c<6; ++c) {
    res.colors[c][0] = (uint8_t)colorsFloat[c][0];
    res.colors[c][1] = (uint8_t)colorsFloat[c][1];
    res.colors[c][2] = (uint8_t)colorsFloat[c][2];
  }

  /*for(int i=0; i<BLEND_COUNT; ++i) {
    if(minProbes[i]) {
      auto &probe = *minProbes[i];
      T3DVec3 probePos{(float)probe.pos[0], (float)probe.pos[1], (float)probe.pos[2]};
      probePos *= 0.15f;
      Debug::drawLine(pos, probePos, {0xFF, 0xFF, 0xFF, 0xFF});
    }
  }*/

  return res;
}
