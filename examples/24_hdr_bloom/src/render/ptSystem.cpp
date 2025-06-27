/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "ptSystem.h"

PTSystem::PTSystem(uint32_t countMax)
  : countMax{countMax}, count{0}
{
  resize(countMax);
}

PTSystem::~PTSystem() {
  if(countMax > 0) {
    free_uncached(mat);
    free_uncached(particles);
  }
}

void PTSystem::resize(uint32_t maxSize)
{
  if(particles) {
    free_uncached(particles);
    particles = nullptr;
  }

  countMax = maxSize;
  assert(sizeof(countMax) % 2 == 0);
  if(countMax > 0) {
    mat = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    particles = static_cast<TPXParticle*>(malloc_uncached(countMax * sizeof(TPXParticle) / 2));
  }
}

void PTSystem::draw() const {
  if(count == 0)return;
  tpx_matrix_push(mat);
  uint32_t safeCount = count & ~1;
  tpx_particle_draw(particles, safeCount);
  tpx_matrix_pop(1);
}

void PTSystem::drawTextured() const {
  if(count == 0)return;
  tpx_matrix_push(mat);
  uint32_t safeCount = count & ~1;
  tpx_particle_draw_tex(particles, safeCount);
  tpx_matrix_pop(1);
}

void PTSystem::drawTexturedSlice(int begin, int end) const
{
  if(begin < 0)begin = 0;
  if(end > (int)countMax)end = countMax;

  begin = begin & ~1; // ensure offset is even
  end = end & ~1; // ensure size is even

  auto size = end - begin;
  if(size <= 0)return;
  tpx_matrix_push(mat);
  tpx_particle_draw_tex(particles + (begin/2), size);
  tpx_matrix_pop(1);
}