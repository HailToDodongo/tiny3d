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
  uint32_t texReference; // dynamic/offscreen texture if non-zero, can be set in fast64
  char* texPath;
  uint32_t textureHash;
  sprite_t* texture;
  uint16_t texWidth;
  uint16_t texHeight;

  T3DMaterialAxis s;
  T3DMaterialAxis t;
} T3DMaterialTexture;

typedef struct {
  uint64_t colorCombiner;
  uint64_t otherModeValue;
  uint64_t otherModeMask;
  uint32_t blendMode;
  uint32_t renderFlags;

  uint8_t _unused00_; // see: T3D_ALPHA_MODE_xxx
  uint8_t fogMode; // see: T3D_FOG_MODE_xxx
  uint8_t setColorFlags;
  uint8_t vertexFxFunc;

  color_t primColor;
  color_t envColor;
  color_t blendColor;

  char* name;
  T3DMaterialTexture textureA;
  T3DMaterialTexture textureB;
} T3DMaterial;

typedef struct {
  T3DVertPacked *vert;
  uint16_t vertLoadCount;
  uint16_t vertDestOffset;

  uint8_t *indices;
  uint16_t numIndices;
  uint16_t matrixIdx;
  uint8_t numStripIndices[4];

} T3DObjectPart;

typedef struct {
  char* name;
  uint16_t numParts;
  uint16_t triCount;
  T3DMaterial* material;
  // can be used freely by the user for recording, will be freed automatically by t3d
  rspq_block_t *userBlock;
  uint8_t isVisible; // set by culling checks, otherwise no effect on rendering
  uint8_t _padding[3];
  int16_t aabbMin[3];
  int16_t aabbMax[3];

  T3DObjectPart parts[]; // real array
} T3DObject;

typedef struct {
  int16_t aabbMin[3];
  int16_t aabbMax[3];
  uint16_t value;
} T3DBvhNode;

typedef struct {
  uint16_t nodeCount;
  uint16_t dataCount;
  T3DBvhNode nodes[];
  // uint16_t data[]; // T3DObject pointer, shifted by 3, relative to 'objectBasePtr'
} T3DBvh;

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
  uint32_t keyframeCount;
  uint16_t channelsQuat;
  uint16_t channelsScalar;
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

  // can be used freely by the user for recording, will be freed automatically by t3d
  rspq_block_t *userBlock;

  int16_t aabbMin[3];
  int16_t aabbMax[3];

  T3DChunkOffset chunkOffsets[];
} T3DModel;

typedef struct {
  union {
    void* chunk;
    T3DObject *object;
    T3DMaterial *material;
    T3DChunkSkeleton *skeleton;
    T3DChunkAnim *anim;
  };

  const T3DModel *_model;
  uint16_t _idx;
  char _chunkType;
} T3DModelIter;

// Types of chunks contained in T3DModel.
enum T3DModelChunkType {
  T3D_CHUNK_TYPE_VERTICES = 'V',
  T3D_CHUNK_TYPE_INDICES  = 'I',
  T3D_CHUNK_TYPE_MATERIAL = 'M',
  T3D_CHUNK_TYPE_OBJECT   = 'O',
  T3D_CHUNK_TYPE_SKELETON = 'S',
  T3D_CHUNK_TYPE_ANIM     = 'A',
  T3D_CHUNK_TYPE_BVH      = 'B'
};

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
 * State for model and material settings during a draw.
 * This is used to minimize state changes across materials.
 */
typedef struct {
  uint32_t lastTextureHashA;
  uint32_t lastTextureHashB;
  uint8_t lastFogMode;
  uint32_t lastRenderFlags;
  uint64_t lastCC;
  color_t lastPrimColor;
  color_t lastEnvColor;
  color_t lastBlendColor;
  uint8_t lastVertFXFunc;
  uint16_t lastUvGenParams[2];
  uint64_t lastOtherMode;
  uint32_t lastBlendMode;
  T3DModelDrawConf* drawConf; // @TODO: legacy, remove at some point
} T3DModelState;

/**
 * Creates a new draw state with default values.
 * @return
 */
static inline T3DModelState t3d_model_state_create() {
  return (T3DModelState){
    .lastFogMode = 0xFF,
    .lastVertFXFunc = T3D_VERTEX_FX_NONE,
    .lastOtherMode = 0xFF,
    .lastBlendMode = 0xFFFFFFFF
  };
}


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
 * Draws an object in a model directly.\n
 * This will only handle the mesh part, and not any material or texture settings.\n
 * As in, it will load vertices (optionally a bone matrix) and then draw the triangles.\n
 * If you want to change material settings, you can use 't3d_model_draw_material' before this.\n
 * Take a look at 't3d_model_iter_create' for an example of how to use it.
 *
 * @param object object to draw
 * @param boneMatrices matrices in the case of skinned meshes, set to NULL for non-skinned
 */
void t3d_model_draw_object(const T3DObject *object, const T3DMat4FP *boneMatrices);

/**
 * Draws/Applies a material of an object. This can be called before 't3d_model_draw_object'.\n
 * This will set up the texture, CC, and other RDP and t3d settings of the material.\n
 *
 * NOTE: if you want to use this to record individual objects into a display list,\n
 * make sure to pass NULL for 'state', otherwise you may only record partial state changes.\n
 * For complete recordings, create a state via 't3d_model_state_create' and pass it to each call.
 *
 * @param mat material to apply
 * @param state state for material settings, used to minimized changes across materials
 */
void t3d_model_draw_material(T3DMaterial *mat, T3DModelState *state);

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
  for(uint32_t i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == T3D_CHUNK_TYPE_SKELETON) {
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
  for(uint32_t i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == T3D_CHUNK_TYPE_ANIM)count++;
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

/**
 * Returns an object by name.
 * @param model model
 * @param name object name
 * @return object or NULL if not found
 */
T3DObject* t3d_model_get_object(const T3DModel *model, const char *name);

/**
 * Returns an object by index.
 * Note that no bounds checking is done, so make sure the index is valid.
 * @param model model
 * @param index object index
 * @return object, invalid pointer if OOB!
 */
static inline T3DObject* t3d_model_get_object_by_index(const T3DModel *model, uint32_t index) {
  uint32_t offset = model->chunkOffsets[index].offset & 0x00FFFFFF;
  return (T3DObject*)((char*)model + offset);
}

/**
 * Returns a material by name.
 * @param model model
 * @param name material name
 * @return material or NULL if not found
 */
T3DMaterial* t3d_model_get_material(const T3DModel *model, const char *name);

/**
 * Creates an iterator to manually traverse the chunks contained in a file.\n
 * This can be used to manually iterate and draw objects.\n
 * \n
 * Note that the iterator does not need to be freed,\n
 * you can simply discard it, or call this function again to rewind it.\n
 * the model it points to must however outlive the iterator.\n
 * \n
 * As an example, here is how to do a custom draw of all objects in a file:
 *
 * @code{.c}
 *  T3DModelIter it = t3d_model_iter_create(itemModel, T3D_CHUNK_TYPE_OBJECT);
 *  while(t3d_model_iter_next(&it)) {
 *    // (Apply materials here before the draw)
 *    t3d_model_draw_object(it.object, NULL);
 *  }
 * @endcode
 *
 * @param model model to iterate
 * @param chunkType type of chunk to iterate (e.g. T3D_CHUNK_TYPE_OBJECT)
 * @return iterator
 */
static inline T3DModelIter t3d_model_iter_create(const T3DModel *model, enum T3DModelChunkType chunkType) {
  return (T3DModelIter){
    .chunk = NULL,
    ._model = model,
    ._idx = 0,
    ._chunkType = chunkType,
  };
}

/**
 * Advances the iterator to the next chunk.\n
 * A pointer to the chunk is stored in 'iter->chunk'.\n
 * If it reached the end, this function will return false and set 'iter->chunk' to NULL.\n
 * \n
 * Depending on the type you passed in via 't3d_model_iter_create', you either have to cast it,\n
 * or directly use one of the typed accessors like 'iter.object' for T3D_CHUNK_TYPE_OBJECT.\n
 *
 * @param iter iterator to advance
 * @return true if it found a chunk, false if it reached the end
 */
bool t3d_model_iter_next(T3DModelIter *iter);

/**
 * Returns the BVH of a model.
 * Note that this is optional and may return NULL.
 * To create one, pass '--bvh' to the gltf importer.
 * @param model model
 * @return pointer to the BVH or NULL if not found
 */
static inline const T3DBvh* t3d_model_bvh_get(const T3DModel *model) {
  for(uint32_t i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == T3D_CHUNK_TYPE_BVH) {
      uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
      return (T3DBvh*)((char*)model + offset);
    }
  }
  return NULL;
}

/**
 * Queries the BVH of a model with a frustum.
 * Note that the BVH is in model space, so the frustum may need to be transformed before.
 * This will mark all objects in the BVH as visible via the 'isVisible' flag.
 * Note that you need to first set all to false before calling this.
 *
 * @param bvh BVH to check
 * @param frustum frustum to check against
 * @return
 */
void t3d_model_bvh_query_frustum(const T3DBvh *bvh, const T3DFrustum *frustum);

#ifdef __cplusplus
}
#endif

#endif