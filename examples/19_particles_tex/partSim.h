#pragma once

static int currentPart  = 0;

static color_t blend_colors(color_t colorA, color_t colorB, float t) {
  color_t color;
  color.r = (uint8_t)(colorA.r * (1.0f - t) + colorB.r * t);
  color.g = (uint8_t)(colorA.g * (1.0f - t) + colorB.g * t);
  color.b = (uint8_t)(colorA.b * (1.0f - t) + colorB.b * t);
  color.a = (uint8_t)(colorA.a * (1.0f - t) + colorB.a * t);
  return color;
}


static color_t get_rainbow_color(float s, float brightness) {
  float r = fm_sinf(s) * 0.5f + 0.5f;
  float g = fm_sinf(s + 2.094f) * 0.5f + 0.5f;
  float b = fm_sinf(s + 4.188f) * 0.5f + 0.5f;
  return (color_t){
    (uint8_t)(r * brightness * 255.0f),
    (uint8_t)(g * brightness * 255.0f),
    (uint8_t)(b * brightness * 255.0f),
    0xFF
  };
}
static color_t get_rand_color(int bright) {
  return (color_t){
    (uint8_t)(bright + (rand() % (256 - bright))),
    (uint8_t)(bright + (rand() % (256 - bright))),
    (uint8_t)(bright + (rand() % (256 - bright))),
    0xFF
  };
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

static float step(float a, float b) {
  return a < b ? 0.0f : 1.0f;
}
static float mix(float a, float b, float t) {
  return a * (1.0f - t) + b * t;
}
static float fract(float a) {
  return a - floorf(a);
}
static float clamp(float a, float min, float max) {
  return a < min ? min : a > max ? max : a;
}
static float map(float value, float oldMin, float oldMax, float newMin, float newMax) {
  return (value - oldMin) / (oldMax - oldMin) * (newMax - newMin) + newMin;
}
static float map01(float value, float newMin, float newMax) {
  return clamp(map(value, 0.0, 1.0, newMin, newMax), 0.0, 1.0);
}
static float easeOutExpo(float x) {
  return x == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * x);
}

float randNoise3d_rand(float coX, float coY){
  coX = fabsf(coX) * 15.1335f;
  coY = fabsf(coY) * 61.15654f;
  coX += coY;
  return fract(fm_sinf(coX) * 65979.1347f);
}

T3DVec3 randNoise3d(float uvX, float uvY) {
  return (T3DVec3){{
    randNoise3d_rand(uvX + 0.23f * 382.567f, uvX + 0.23f * 382.567f),
    randNoise3d_rand(uvY + 0.65f * 330.356f, uvX + 0.65f * 330.356f),
    randNoise3d_rand(uvX + 0.33f * 356.346f, uvY + 0.33f * 356.346f)
  }};
}

static uint32_t seed = 0x12345678;
static uint32_t rand_local() {
  seed = (seed * 1664525) + 1013904223;
  return seed;
}

static void generate_particles_explosion(TPXParticle *particles, uint32_t count, float time) {
  float timeNorm = time / 2.0f;
  timeNorm = fmodf(timeNorm, 1.0f);

  seed = 0x12345678;

  for (int i = 0; i < count; i++) {
    int8_t *ptPos = tpx_buffer_get_pos(particles, i);
    uint8_t *ptColor = tpx_buffer_get_rgba(particles, i);
    int8_t *ptSize = tpx_buffer_get_size(particles, i);

    float partSize = 1.0f;
    T3DVec3 partPos = {{0.0f, 0.0f, 0.0f}};

    //debugf("Time: %f\n", timeNorm);
    gradient_fire(ptColor, timeNorm);
    ptColor[3] = (rand_local() % 8) * 32;


      int partType = rand_local() % 3;
      // Ring
      float randSeedA = (float)(rand_local() % 512) * (1.0f / 256.0f) - 1.0f;
      float randSeedB = (float)(rand_local() % 512) * (1.0f / 256.0f) - 1.0f;
      float randSeed01 = (float)(rand_local() % 512) * (1.0f / 512.0f);
      float timeNormExpo = easeOutExpo(timeNorm);

      if(partType == 0) // Ring
      {
        partPos = (T3DVec3){{randSeedA, 0.0f, randSeedB}};
        t3d_vec3_norm(&partPos);
        float radius = timeNormExpo * (0.8f + randSeed01 * 0.2f);
        t3d_vec3_scale(&partPos, &partPos, radius);
        partSize = (easeOutExpo(1.0f - timeNorm));
      } else // Sphere
      {
        // sample points of the surface of a sphere
        float theta = randNoise3d_rand(randSeedA, randSeedB) * 2.0f * T3D_PI;
        float phi = randNoise3d_rand(randSeedB, randSeedA) * T3D_PI;

        partPos.x = fm_sinf(phi) * fm_cosf(theta);
        partPos.y = fm_cosf(phi);
        partPos.z = fm_sinf(phi) * fm_sinf(theta);
        t3d_vec3_scale(&partPos, &partPos, timeNormExpo * 0.4f);
        partSize = (easeOutExpo(1.0f - timeNorm)) * 1.25f;
      }

      ptSize[0] = partSize * 80.0f;
      ptPos[0] = partPos.x * 127;
      ptPos[1] = partPos.y * 127;
      ptPos[2] = partPos.z * 127;

      // color
      //fragColor.rgb *= 1.0 - map01(timeNormLoopEase, -0.75, 1.0);
      //fragColor.rgb *= 1.0 + (typeSphere * 0.5);
      //fragColor.g *= max(typeSphere, 0.5);

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
  int heightParts = 1;
  int sideLen = roundf(sqrt(partCount / heightParts)) + 1;

  int p = 0;

  int8_t ptPosX = -(dist * sideLen) / 2;
  for(int x=0; x<sideLen; ++x)
  {
    int8_t ptPosZ = -(dist * sideLen) / 2;
    for(int z=0; z<sideLen; ++z)
    {
      int8_t *ptPos = tpx_buffer_get_pos(particles, p);
      color_t *ptColor = (color_t*)tpx_buffer_get_rgba(particles, p);

      int rnd = noise_2d(x, z);
      float height = fm_sinf((x + z) * 0.1f) * 0.5f + 0.5f;
      height += fm_cosf((x + z) * 0.13f) * 0.5f + 0.5f;
      //height += 4;
      int8_t size = (rand() % 8)*4 + 50;

      //*ptColor = (rnd & 1) ? get_rand_color(20) : get_rainbow_color((x + z) * 0.1f, 1.0f);


      *ptColor = blend_colors(
          (color_t){0xAA, 0xFF, 0x55, 0xFF},
          (color_t){0x55, 0xAA, 0x00, 0xFF},
          (rnd & 100) * 0.1f
        );


      ptColor->a = (rand() % 8) * 32;

      ptPos[0] = ptPosX + ((rnd % 3) - 1);
      ptPos[1] = height;
      ptPos[2] = ptPosZ + ((rnd % 3) - 1);
      *tpx_buffer_get_size(particles, p) = size;

      ptPosZ += dist;

      if(++p >= partCount)goto LOOP_END;
    }
    ptPosX += dist;
  }
  LOOP_END:

  return p & ~1;
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
    color[3] = (PhysicalAddr(ptPos) % 8) * 32;

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
