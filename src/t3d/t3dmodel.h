/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#ifndef TINY3D_T3DMODEL_H
#define TINY3D_T3DMODEL_H

#include "t3d.h"

typedef struct {
  float low;
  float height;
  int8_t mask;
  int8_t shift;
  uint8_t mirror;
  uint8_t clamp;
} T3DMaterialAxis;

typedef struct {
  uint64_t colorCombiner;
  uint32_t renderFlags;

  char* texPath;
  uint32_t textureHash;
  sprite_t* texture;

  uint16_t texWidth;
  uint16_t texHeight;

  T3DMaterialAxis s;
  T3DMaterialAxis t;
} T3DMaterial;

static_assert(sizeof(T3DMaterial));

typedef struct {
  T3DVertPacked *vert;
  uint32_t vertLoadCount;

  uint8_t *indices;
  uint16_t numIndices;
  uint8_t indexType;
  uint8_t _padding;

} T3DObjectPart;

typedef struct {
  uint32_t numParts;
  T3DMaterial* materialA;
  T3DMaterial* materialB;

  T3DObjectPart parts[]; // real array
} T3DObject;

typedef union {
  char type;
  uint32_t offset;
} T3DChunkOffset;

typedef struct {
  char magic[4];
  uint32_t chunkCount;

  uint32_t chunkIdxVertices;
  uint32_t chunkIdxIndices;
  uint32_t chunkIdxMaterials;
  char* stringTablePtr;

  T3DChunkOffset chunkOffsets[];
} T3DModel;

T3DModel* t3d_model_load(const void *path);

void t3d_model_free(T3DModel* model);

void t3d_model_draw(const T3DModel* model);

#endif