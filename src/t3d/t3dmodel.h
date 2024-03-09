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

  uint16_t totalVertCount;
  uint16_t totalIndexCount;

  uint32_t chunkIdxVertices;
  uint32_t chunkIdxIndices;
  uint32_t chunkIdxMaterials;
  char* stringTablePtr;

  T3DChunkOffset chunkOffsets[];
} T3DModel;

/**
 * Loads a model from a file.
 * If you no longer need the model, call 't3d_model_free'
 *
 * @param path FS path
 * @return pointer to the model (that you now own)
 */
T3DModel* t3d_model_load(const void *path);

// callback for custom drawing, this hooks into the tile-setting section
typedef void (*T3DModelTileCb)(void* userData, rdpq_texparms_t *tileParams, rdpq_tile_t tile);

// Defines settings and callbacks for custom drawing
typedef struct {
  void* userData;
  T3DModelTileCb tileCb;
} T3DModelDrawConf;

/**
 * Free model and any related resources (e.g. textures)
 * @param model
 */
void t3d_model_free(T3DModel* model);

/**
 * Draws a model with a custom configuration.
 * This call can be recorded into a display list.
 * @param model model to draw
 * @param conf custom configuration
 */
void t3d_model_draw_custom(const T3DModel* model, T3DModelDrawConf conf);

/**
 * Draws a model with default settings.
 * This call can be recorded into a display list.
 * @param model model to draw
 */
static inline void t3d_model_draw(const T3DModel* model) {
  t3d_model_draw_custom(model, (T3DModelDrawConf){});
}

/**
 * Returns the global vertex buffer of a model.
 * For the amount of vertices, see 'model->totalVertCount'.
 * Since vertices are interleaved, the max index is 'totalVertCount / 2'.
 * For easy access, use functions like 't3d_vertbuffer_get_pos' and similar.
 *
 * Note that this is shared for all parts of the model.
 * If you need to change specific vertices, you must iterate over the parts.
 *
 * @param model
 * @return pointer to the vertex buffer
 */
static inline T3DVertPacked* t3d_model_get_vertices(const T3DModel *model) {
  uint32_t offset = model->chunkOffsets[model->chunkIdxVertices].offset & 0x00FFFFFF;
  return (void*)model + offset;
}

#endif