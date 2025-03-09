#include <cstdint>
#include <array>
#include <limits>
#include <string>
#include <iostream>
#include <fstream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

namespace {
  FILE* fOut{nullptr};

  constexpr float SCALE_FACTOR = 64.0f;

  struct Probe
  {
    int16_t pos[3];
    uint8_t color[6][3];
  };

  void writeU16(uint16_t val)
  {
    uint8_t high = (val >> 8) & 0xFF;
    uint8_t low = val & 0xFF;
    fwrite(&high, 1, 1, fOut);
    fwrite(&low, 1, 1, fOut);
  }

  void writeU8(uint8_t val) {
    fwrite(&val, 1, 1, fOut);
  }
  void writeU32(uint32_t val) {
    uint8_t b0 = (val >> 24) & 0xFF;
    uint8_t b1 = (val >> 16) & 0xFF;
    uint8_t b2 = (val >> 8) & 0xFF;
    uint8_t b3 = val & 0xFF;
    fwrite(&b0, 1, 1, fOut);
    fwrite(&b1, 1, 1, fOut);
    fwrite(&b2, 1, 1, fOut);
    fwrite(&b3, 1, 1, fOut);
  }
}

int main(int argc, char* argv[])
{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <lightProbes.json>" << endl;
    return 1;
  }

  std::vector<Probe> probes{};

  std::ifstream ifs(argv[1]);
  auto data = json::parse(ifs);
  printf("JSON %s\n", data.dump(2).c_str());

  for (auto& probe : data) {
    Probe p{};
    p.pos[0] = probe["pos"][0].get<float>() * SCALE_FACTOR;
    p.pos[1] = probe["pos"][1].get<float>() * SCALE_FACTOR;
    p.pos[2] = probe["pos"][2].get<float>() * SCALE_FACTOR;
    for(int c=0; c<6; ++c) {
      p.color[c][0] = probe["color"][c][0].get<uint8_t>();
      p.color[c][1] = probe["color"][c][1].get<uint8_t>();
      p.color[c][2] = probe["color"][c][2].get<uint8_t>();
    }
    probes.push_back(p);

    printf("Probe: %d %d %d\n", p.pos[0], p.pos[1], p.pos[2]);
  }

  fOut = fopen(argv[2], "wb");
  writeU32(probes.size());
  for (auto& probe : probes) {
    writeU16(probe.pos[0]);
    writeU16(probe.pos[1]);
    writeU16(probe.pos[2]);
    for(int c=0; c<6; ++c) {
      writeU8(probe.color[c][0]);
      writeU8(probe.color[c][1]);
      writeU8(probe.color[c][2]);
    }
  }
  fclose(fOut);
  return 0;
}
