/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include <string>
#include <vector>
#include <t3d/t3dmath.h>

struct LightProbe
{
  int16_t pos[3]{};
  uint8_t colors[6][3]{};
};

struct LightProbeRes {
  uint8_t colors[6][3]{};
};

class LightProbes
{
  private:
    std::vector<LightProbe> probes{};

  public:
    LightProbes(const std::string &path);

    LightProbeRes query(const T3DVec3 &pos) const;
};