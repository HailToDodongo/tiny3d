/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#ifndef TINY3D_T3DMODEL_H
#define TINY3D_T3DMODEL_H

#include "t3d.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define T3D_ALPHA_MODE_DEFAULT 0
#define T3D_ALPHA_MODE_OPAQUE  1
#define T3D_ALPHA_MODE_CUTOUT  2
#define T3D_ALPHA_MODE_TRANSP  3

#define T3D_FOG_MODE_DEFAULT  0
#define T3D_FOG_MODE_DISABLED 1
#define T3D_FOG_MODE_ACTIVE   2

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

  uint8_t alphaMode; // see: T3D_ALPHA_MODE_xxx
  uint8_t fogMode; // see: T3D_FOG_MODE_xxx
  uint8_t _reserved[2];

  uint32_t texReference; // dynamic/offscreen texture if non-zero, can be set in fast64
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
  uint16_t vertLoadCount;
  uint16_t vertDestOffset;

  uint8_t *indices;
  uint16_t numIndices;
  uint16_t matrixIdx;

} T3DObjectPart;

typedef struct {
  char* name;
  uint32_t numParts;
  T3DMaterial* materialA;
  T3DMaterial* materialB;

  T3DObjectPart parts[]; // real array
} T3DObject;

typedef struct {
  char* name;
  uint16_t parentIdx;
  uint16_t depth;
  T3DVec3 scale;
  T3DQuat rotation;
  T3DVec3 position;
} T3DChunkBone;

typedef struct {
  uint16_t boneCount;
  uint16_t _reserved;
  T3DChunkBone bones[];
} T3DChunkSkeleton;

typedef struct {
  uint16_t targetIdx;
  uint8_t targetType;
  uint8_t attributeIdx;
  float quantScale;
  float quantOffset;
} T3DAnimChannelMapping;

typedef struct {
  char* name;
  float duration;
  uint16_t keyframeCount;
  uint16_t channelCount;
  char* filePath;
  T3DAnimChannelMapping channelMappings[];
} T3DChunkAnim;

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
T3DModel* t3d_model_load(const char *path);

// callback for custom drawing, this hooks into the tile-setting section
typedef void (*T3DModelTileCb)(void* userData, rdpq_texparms_t *tileParams, rdpq_tile_t tile);
typedef bool (*T3DModelFilterCb)(void* userData, const T3DObject *obj);
typedef void (*T3DModelDynTextureCb)(
  void* userData, const T3DMaterial *material, rdpq_texparms_t *tileParams, rdpq_tile_t tile
);

// Defines settings and callbacks for custom drawing
typedef struct {
  void* userData;
  T3DModelTileCb tileCb; // callback to modify tile settings
  T3DModelFilterCb filterCb; // callback to filter parts
  T3DModelDynTextureCb dynTextureCb; // callback to set dynamic textures, aka "Texture Reference" in fast64
  const T3DMat4FP *matrices;
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
  t3d_model_draw_custom(model, (T3DModelDrawConf){
    .userData = NULL,
    .tileCb = NULL,
    .filterCb = NULL
  });
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
  return (T3DVertPacked*)((char*)model + offset);
}

/**
 * Returns the first/main skeleton of a model.
 * If the model contains multiple skeletons, data must be manually traversed instead.
 *
 * @param model model
 * @return pointer to the skeleton or NULL if not found
 */
static inline const T3DChunkSkeleton* t3d_model_get_skeleton(const T3DModel *model) {
  for(int i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == 'S') {
      uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
      return (T3DChunkSkeleton*)((char*)model + offset);
    }
  }
  return NULL;
}

/**
 * Returns the number of animations in the model.
 * @param model
 * @return
 */
static inline uint32_t t3d_model_get_animation_count(const T3DModel *model) {
  uint32_t count = 0;
  for(int i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == 'A')count++;
  }
  return count;
}

/**
 * Stores the pointers to all animations inside the model into `anims`.
 * Make sure to use `t3d_model_get_animation_count` to allocate enough memory.
 * @param model
 * @param anims array to store the pointers to
 */
void t3d_model_get_animations(const T3DModel *model, T3DChunkAnim **anims);

/**
 * Returns an animation definition by name.
 * Note: if you want to create an animation instance, use 't3d_anim_create'.
 *
 * @param model
 * @param name animation name
 * @return pointer to the animation or NULL if not found
 */
T3DChunkAnim* t3d_model_get_animation(const T3DModel *model, const char* name);

#ifdef __cplusplus
}
#endif

#endif