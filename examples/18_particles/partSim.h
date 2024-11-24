#pragma once

static int currentPart  = 0;

/**
 * Basic static particles with random positions and colors.
 */
static void generate_particles_random(TPXParticle *particles, uint32_t count) {
  for (int i = 0; i < count; i++) {
    int p = i / 2;
    int8_t *ptPos = i % 2 == 0 ? particles[p].posA : particles[p].posB;
    uint8_t *ptColor = i % 2 == 0 ? particles[p].colorA : particles[p].colorB;

    particles[p].sizeA = 20 + (rand() % 10);
    particles[p].sizeB = 20 + (rand() % 10);

    T3DVec3 pos = {{
       (i * 1 + rand()) % 128 - 64,
       (i * 3 + rand()) % 128 - 64,
       (i * 4 + rand()) % 128 - 64
     }};

    t3d_vec3_norm(&pos);
    float len = rand() % 40;
    pos.v[0] *= len;
    pos.v[1] *= len;
    pos.v[2] *= len;

    ptPos[0] = (rand() % 256) - 128;
    ptPos[1] = (rand() % 256) - 128;
    ptPos[2] = (rand() % 256) - 128;

    ptColor[3] = rand() % 8;
    ptColor[0] = 25 + (rand() % 230);
    ptColor[1] = 25 + (rand() % 230);
    ptColor[2] = 25 + (rand() % 230);
  }
}

static int noise_2d(int x, int y) {
  int n = x + y * 57;
  n = (n << 13) ^ n;
  return (n * (n * n * 60493 + 19990303) + 89);
}

/**
 * Static particles simulating grass.
 * This will create a random grid of 3 particles stacked on top of each other representing grass-blades.
 */
static int simulate_particles_grass(TPXParticle *particles, uint32_t partCount) {

  int dist = 3;
  int heightParts = 3;
  int sideLen = roundf(sqrt(partCount / heightParts)) + 1;

  int p = 0;
  for(int y=heightParts-1; y>=0; --y)
  {
    int8_t ptPosX = -(dist * sideLen) / 2;
    for(int x=0; x<sideLen; ++x)
    {
      int8_t ptPosZ = -(dist * sideLen) / 2;
      for(int z=0; z<sideLen; ++z)
      {
        int8_t *ptPos = tpx_buffer_get_pos(particles, p);
        uint8_t *ptColor = tpx_buffer_get_rgba(particles, p);

        int rnd = noise_2d(x, z);
        float height = fm_sinf((x + z) * 0.1f) * 0.5f + 0.5f;
        height += fm_cosf((x + z) * 0.13f) * 0.5f + 0.5f;
        int8_t size = ((heightParts-y) * 6) + 6 + height;

        ptColor[0] = ptColor[1] = ptColor[2] = (100 + y*50) + (rnd % 55);

        ptPos[0] = ptPosX + ((rnd % 3) - 1);
        ptPos[1] = y + height;
        ptPos[2] = ptPosZ + ((rnd % 3) - 1);
        *tpx_buffer_get_size(particles, p) = size;

        ptPosZ += dist;

        if(++p >= partCount)goto LOOP_END;
      }
      ptPosX += dist;
    }
  }
  LOOP_END:

  return p & ~1;
}

// Fire color: white -> yellow/orange -> red -> black
static void gradient_fire(uint8_t *color, float t) {
    t = fminf(1.0f, fmaxf(0.0f, t));
    t = 0.8f - t;
    t *= t;

    if (t < 0.25f) { // Dark red to bright red
      color[0] = (uint8_t)(200 * (t / 0.25f)) + 55;
      color[1] = 0;
      color[2] = 0;
    } else if (t < 0.5f) { // Bright red to yellow
      color[0] = 255;
      color[1] = (uint8_t)(255 * ((t - 0.25f) / 0.25f));
      color[2] = 0;
    } else if (t < 0.75f) { // Yellow to white (optional, if you want a bright white center)
      color[0] = 255;
      color[1] = 255;
      color[2] = (uint8_t)(255 * ((t - 0.5f) / 0.25f));
    } else { // White to black
      color[0] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
      color[1] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
      color[2] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
    }
}

/**
 * Particle system for a fire effect.
 * This will simulate particles over time by moving them up and changing their color.
 * The current position is used to spawn new particles, so it can move over time leaving a trail behind.
 */
static void simulate_particles_fire(TPXParticle *particles, uint32_t partCount, float posX, float posZ) {
  uint32_t p = currentPart / 2;
  if(currentPart % (1+(rand() % 3)) == 0) {
    int8_t *ptPos = currentPart % 2 == 0 ? particles[p].posA : particles[p].posB;
    int8_t *size = currentPart % 2 == 0 ? &particles[p].sizeA : &particles[p].sizeB;
    uint8_t *color = currentPart % 2 == 0 ? particles[p].colorA : particles[p].colorB;

    ptPos[0] = posX + (rand() % 16) - 8;
    ptPos[1] = -126;
    gradient_fire(color, 0);
    ptPos[2] = posZ + (rand() % 16) - 8;
    *size = 60 + (rand() % 10);
  }
  currentPart = (currentPart + 1) % partCount;

  // move all up by one unit
  for (int i = 0; i < partCount/2; i++) {
    gradient_fire(particles[i].colorA, (particles[i].posA[1] + 127) / 150.0f);
    gradient_fire(particles[i].colorB, (particles[i].posB[1] + 127) / 150.0f);

    particles[i].posA[1] += 1;
    particles[i].posB[1] += 1;
    if(currentPart % 4 == 0) {
      particles[i].sizeA -= 2;
      particles[i].sizeB -= 2;
      if(particles[i].sizeA < 0)particles[i].sizeA = 0;
      if(particles[i].sizeB < 0)particles[i].sizeB = 0;
    }
  }
}

static color_t blend_colors(color_t colorA, color_t colorB, float t) {
  color_t color;
  color.r = (uint8_t)(colorA.r * (1.0f - t) + colorB.r * t);
  color.g = (uint8_t)(colorA.g * (1.0f - t) + colorB.g * t);
  color.b = (uint8_t)(colorA.b * (1.0f - t) + colorB.b * t);
  color.a = (uint8_t)(colorA.a * (1.0f - t) + colorB.a * t);
  return color;
}
