/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include <t3d/t3d.h>
#include <t3d/tpx.h>

struct PTSystem
{
  T3DVec3 pos{};
  T3DMat4FP *mat{};
  TPXParticle *particles{};
  uint32_t countMax{};
  uint32_t count{};

  PTSystem(uint32_t maxSize = 0);
  ~PTSystem();

  void resize(uint32_t maxSize);
  [[nodiscard]] bool isFull() const { return count == countMax; }

  void removeParticle(uint32_t index) {
    tpx_buffer_copy(particles, index, --count);
    if(count & 1) {
      *tpx_buffer_get_size(particles, count + 1u) = 0;
    }
  }

  void draw() const;
  void drawTextured() const;

  void drawTexturedSlice(int begin, int end) const;
};