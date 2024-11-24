/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "t3dmodel.h"

#define T3DM_VERSION 0x03

static inline void* patch_pointer(void *ptr, uint32_t offset) {
  return (void*)(offset + (int32_t)ptr);
}

static inline void* align_pointer(void *ptr, uint32_t alignment) {
  return (void*)(((uint32_t)ptr + alignment - 1) & ~(alignment - 1));
}

static inline bool is_power_of_two(uint16_t x) {
  return (x & (x - 1)) == 0;
}

typedef struct {
  uint16_t objectPtr;
} T3DBvhData;

typedef struct {
  uint32_t hash;
  sprite_t *texture;
  uint32_t count;
} T3DTextureEntry;

static uint32_t textureCacheSize = 0;
static T3DTextureEntry *textureCache = NULL;
static T3DModelState dummyState;

static sprite_t* texture_cache_get(uint32_t hash) {
  for(uint32_t i = 0; i < textureCacheSize; i++) {
    if(textureCache[i].hash == hash) {
      textureCache[i].count++;
      return textureCache[i].texture;
    }
  }
  return NULL;
}

static void texture_cache_add(uint32_t hash, sprite_t *texture) {
  T3DTextureEntry *cacheEntry = NULL;
  for(uint32_t i = 0; i < textureCacheSize; i++) {
    if(textureCache[i].hash == 0) {
      cacheEntry = &textureCache[i];
      break;
    }
  }

  if(cacheEntry == NULL) {
    textureCacheSize++;
    if(textureCache == NULL) {
      textureCache = malloc(sizeof(T3DTextureEntry));
    } else {
      textureCache = realloc(textureCache, sizeof(T3DTextureEntry) * textureCacheSize);
    }
    cacheEntry = &textureCache[textureCacheSize-1];
  }

  cacheEntry->hash = hash;
  cacheEntry->texture = texture;
  cacheEntry->count = 1;
}

static void texture_cache_free(uint32_t hash)
{
  for(uint32_t i = 0; i < textureCacheSize; i++) {
    if(textureCache[i].hash == hash) {
      textureCache[i].count--;
      //debugf("Free Texture: %08lX, count=%lu\n", hash, textureCache[i].count);
      if(textureCache[i].count == 0) {
        sprite_free(textureCache[i].texture);
        textureCache[i].hash = 0;
      }
      return;
    }
  }
}

static void texture_cache_free_mem()
{
  uint32_t emptyEntries = 0;

  for(uint32_t i = 0; i < textureCacheSize; i++) {
    if(textureCache[i].hash == 0) {
      emptyEntries++;
    }
  }

  if(textureCache && emptyEntries == textureCacheSize)
  {
    free(textureCache);
    textureCache = NULL;
    textureCacheSize = 0;
  }
}

static void set_texture(T3DMaterial *mat, rdpq_tile_t tile, T3DModelDrawConf *conf)
{
  T3DMaterialTexture *tex = tile == TILE0 ? &mat->textureA : &mat->textureB;
  if(tex->texPath || tex->texReference)
  {
    //debugf("Load Texture: %s (%08lX)\n", tex->texPath, tex->textureHash);
    if(tex->texPath && !tex->texture) {
      tex->texture = texture_cache_get(tex->textureHash);
      if(tex->texture == NULL) {
        //debugf("Not in cache, load %s (%08lX)\n", tex->texPath, tex->textureHash);
        tex->texture = sprite_load(tex->texPath);
        //const char* formatName = tex_format_name(sprite_get_format(mat->texture));
        //debugf(" -> %s\n", formatName);
        texture_cache_add(tex->textureHash, tex->texture);
      }
    }

    rdpq_texparms_t texParam = (rdpq_texparms_t){};
    texParam.s.translate = tex->s.low;
    texParam.s.mirror = tex->s.mirror;
    texParam.s.repeats = REPEAT_INFINITE;
    texParam.s.scale_log = (int)tex->s.shift;

    if(tex->s.clamp) {
      if(is_power_of_two(tex->texWidth)) {
        texParam.s.repeats = (tex->s.height+1.0f) / (float)tex->texWidth;
      } else {
        texParam.s.repeats = 1;
      }
    }

    texParam.t.translate = tex->t.low;
    texParam.t.mirror = tex->t.mirror;
    texParam.t.repeats = REPEAT_INFINITE;
    texParam.t.scale_log = (int)tex->t.shift;

    if(tex->t.clamp) {
      if(is_power_of_two(tex->texHeight)) {
        texParam.t.repeats = (tex->t.height+1.0f) / (float)tex->texHeight;
      } else {
        texParam.t.repeats = 1;
      }
    }

    if(conf && conf->tileCb) {
      conf->tileCb(conf->userData, &texParam, tile);
    }

    // @TODO: don't upload texture if only the tile settings differ
    if(tex->texReference) {
      if(conf && conf->dynTextureCb)conf->dynTextureCb(conf->userData, mat, &texParam, tile);
    } else {
      rdpq_sync_tile();
      if(tile == TILE1 && mat->textureA.textureHash == mat->textureB.textureHash) {
        rdpq_tex_reuse(TILE1, &texParam);
      } else {
        rdpq_sprite_upload(tile, tex->texture, &texParam);
      }
    }
  }
}

static bool handle_bone_matrix(const T3DObjectPart *part, const T3DMat4FP* matStack, bool hadMatrixPush)
{
  if(matStack) {
    if(part->matrixIdx != 0xFFFF) {
      if(!hadMatrixPush) {
        t3d_matrix_push(&matStack[part->matrixIdx]);
      } else {
        t3d_matrix_set(&matStack[part->matrixIdx], true);
      }
      hadMatrixPush = true;
    } else {
      if(hadMatrixPush) {
        t3d_matrix_pop(1);
        hadMatrixPush = false;
      }
    }
  }
  return hadMatrixPush;
}

T3DModel *t3d_model_load(const char *path) {
  int size = 0;
  T3DModel* model = asset_load(path, &size);
  int32_t ptrOffset = (int32_t)(void*)model;

  if(memcmp(model->magic, "T3M", 3) != 0) {
    assertf(false, "Invalid T3D model file: %s", path);
  }
  assertf(model->magic[3] == T3DM_VERSION,
    "Invalid T3D model version: %d != %d\n"
    "Please make a clean build of t3d and your project",
    T3DM_VERSION, model->magic[3]);

  void* basePtrVertices = (char*)model + (model->chunkOffsets[model->chunkIdxVertices].offset & 0xFFFFFF);
  void* basePtrIndices = (char*)model + (model->chunkOffsets[model->chunkIdxIndices].offset & 0xFFFFFF);
  model->stringTablePtr = patch_pointer(model->stringTablePtr, ptrOffset);

  for(uint32_t i = 0; i < model->chunkCount; i++)
  {
    char chunkType = model->chunkOffsets[i].type;
    uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
    //debugf("Chunk[%lu] '%c': %lx\n", i, chunkType, (uint32_t)offset);

    if(chunkType == T3D_CHUNK_TYPE_OBJECT) {
      T3DObject *obj = (T3DObject*)((char*)model + offset);
      if(obj->name != NULL) {
        obj->name = patch_pointer(obj->name, (uint32_t)model->stringTablePtr);
      }

      uint32_t matIdx = model->chunkIdxMaterials + (uint32_t)obj->material;
      obj->material = (T3DMaterial*)((char*)model + (model->chunkOffsets[matIdx].offset & 0xFFFFFF));

      for(uint32_t j = 0; j < obj->numParts; j++) {
        T3DObjectPart *part = &obj->parts[j];
        part->indices = patch_pointer(part->indices, (uint32_t)basePtrIndices);
        part->vert = patch_pointer(part->vert, (uint32_t)basePtrVertices);

        uint8_t *stripPtr = align_pointer(part->indices + part->numIndices, 8);
        for(int s=0; s<4; ++s) {
          if(part->numStripIndices[s] == 0)break;
          t3d_indexbuffer_convert((int16_t*)stripPtr, part->numStripIndices[s]);
          stripPtr = (uint8_t*)align_pointer(stripPtr + part->numStripIndices[s]*2, 8);
        }
      }
    }

    if(chunkType == T3D_CHUNK_TYPE_MATERIAL) {
      T3DMaterial *mat = (T3DMaterial*)((char*)model + offset);

      if(mat->name)mat->name += (uint32_t)model->stringTablePtr;
      if(mat->textureA.texPath)mat->textureA.texPath += (uint32_t)model->stringTablePtr;
      if(mat->textureB.texPath)mat->textureB.texPath += (uint32_t)model->stringTablePtr;
    }

    if(chunkType == T3D_CHUNK_TYPE_SKELETON) {
      T3DChunkSkeleton *skel = (T3DChunkSkeleton*)((char*)model + offset);
      for(int j = 0; j < skel->boneCount; j++) {
        T3DChunkBone *bone = &skel->bones[j];
        bone->name = patch_pointer(bone->name, (uint32_t)model->stringTablePtr);
      }
    }

    if(chunkType == T3D_CHUNK_TYPE_ANIM) {
      T3DChunkAnim *anim = (T3DChunkAnim*)((char*)model + offset);
      anim->name = patch_pointer(anim->name, (uint32_t)model->stringTablePtr);
      anim->filePath = patch_pointer(anim->filePath, (uint32_t)model->stringTablePtr);
    }

    if(chunkType == T3D_CHUNK_TYPE_BVH) {
      // node leafs are stored as indices to the objects, we convert that to an relative address
      // to the actual object, shifted by 2 since it's 4 byte aligned (and nodes use 16bit indices)
      T3DBvh *bvh = (T3DBvh*)((char*)model + offset);
      T3DBvhData *data = (T3DBvhData*)&bvh->nodes[bvh->nodeCount]; // data is right after nodes

      for(int d=0; d<bvh->dataCount; ++d) {
        T3DObject *obj = t3d_model_get_object_by_index(model, data[d].objectPtr);
        uint32_t addr = (uint32_t)bvh  - (uint32_t)obj;
        assert((addr & 0b11) == 0);
        addr >>= 2;
        assert(addr < 0x10000);
        data[d].objectPtr = addr;
      }
    }
  }

  data_cache_hit_writeback_invalidate(model, size);
  return model;
}

void t3d_model_draw_custom(const T3DModel* model, T3DModelDrawConf conf)
{
  T3DModelState state = t3d_model_state_create();
  state.drawConf = &conf;

  T3DModelIter it = t3d_model_iter_create(model, T3D_CHUNK_TYPE_OBJECT);
  while(t3d_model_iter_next(&it))
  {
    if(conf.filterCb && !conf.filterCb(conf.userData, it.object)) {
      continue;
    }

    if(it.object->material) {
      t3d_model_draw_material(it.object->material, &state);
    }
    t3d_model_draw_object(it.object, conf.matrices);
  }

  if(state.lastVertFXFunc != T3D_VERTEX_FX_NONE)t3d_state_set_vertex_fx(T3D_VERTEX_FX_NONE, 0, 0);
}

void t3d_model_draw_object(const T3DObject *object, const T3DMat4FP *boneMatrices)
{
  bool hadMatrixPush = false;
  for(uint32_t p = 0; p < object->numParts; p++)
  {
    const T3DObjectPart *part = &object->parts[p];
    hadMatrixPush = handle_bone_matrix(part, boneMatrices, hadMatrixPush);

    // load vertices, this will already do T&L (so matrices/fog/lighting must be set before)
    t3d_vert_load(part->vert, part->vertDestOffset, part->vertLoadCount);
    //debugf("Load Vertices[%d]: %d, %d | bone: %d\n", p, part->vertDestOffset, part->vertLoadCount, part->matrixIdx);
    if(part->numIndices == 0 && part->numStripIndices[0] == 0)continue; // partial-load, last chunk of a sequence will both indices & material data

    // Draw single triangles first...
    for(int i = 0; i < part->numIndices; i+=3) {
      t3d_tri_draw(part->indices[i], part->indices[i+1], part->indices[i+2]);
    }

    // ...then strips, which are an encoded index buffer DMA'd by the RSP.
    // Internally, this will re-use the space of the vertex cache of verts. that are not used anymore
    uint8_t *idxPtrBase = (uint8_t*)align_pointer(part->indices + part->numIndices, 8);
    for(int s=0; s<4; ++s) {
      if(part->numStripIndices[s] == 0)break;
      t3d_tri_draw_strip((int16_t*)idxPtrBase, part->numStripIndices[s]);
      idxPtrBase = (uint8_t*)align_pointer(idxPtrBase + part->numStripIndices[s] * 2, 8);
    }

    // Sync, waits for any triangles in flight. This is necessary since the rdpq-api is not
    // aware of this and could corrupt the RDP buffer. 't3d_vert_load' may also overwrite the source buffer too.
    t3d_tri_sync();

    // At this point the RDP may already process triangles.
    // In the next iteration we may therefore need to sync when changing any RDP states
  }

  if(hadMatrixPush)t3d_matrix_pop(1);
}

void t3d_model_draw_material(T3DMaterial *mat, T3DModelState *state)
{
  if(!state) {
    dummyState = t3d_model_state_create();
    state = &dummyState;
  }

  if(mat->renderFlags != state->lastRenderFlags) {
    t3d_state_set_drawflags(mat->renderFlags);
    state->lastRenderFlags = mat->renderFlags;
  }

  if(mat->fogMode != T3D_FOG_MODE_DEFAULT && mat->fogMode != state->lastFogMode) {
    state->lastFogMode = mat->fogMode;
    t3d_fog_set_enabled(mat->fogMode == T3D_FOG_MODE_ACTIVE);
  }

  if(state->lastVertFXFunc != mat->vertexFxFunc || (
    mat->vertexFxFunc && (state->lastUvGenParams[0] != mat->textureA.texWidth || state->lastUvGenParams[1] != mat->textureA.texHeight)
  )) {
    state->lastVertFXFunc = mat->vertexFxFunc;
    state->lastUvGenParams[0] = mat->textureA.texWidth;
    state->lastUvGenParams[1] = mat->textureA.texHeight;
    t3d_state_set_vertex_fx(state->lastVertFXFunc, (int16_t)state->lastUvGenParams[0], (int16_t)state->lastUvGenParams[1]);
  }

  // now apply rdpq settings, these are independent of the t3d state
  // and only need to happen before a `t3d_tri_draw` call
  if(mat->colorCombiner)
  {
    bool setBlendMode  = state->lastBlendMode != mat->blendMode;
    bool setCC         = mat->colorCombiner != state->lastCC;
    bool setTexture    = state->lastTextureHashA != mat->textureA.textureHash || state->lastTextureHashB != mat->textureB.textureHash;
    bool setOtherMode  = state->lastOtherMode != mat->otherModeValue || setTexture;
    bool setPrimColor  = (mat->setColorFlags & 0b001) && color_to_packed32(state->lastPrimColor) != color_to_packed32(mat->primColor);
    bool setEnvColor   = (mat->setColorFlags & 0b010) && color_to_packed32(state->lastEnvColor) != color_to_packed32(mat->envColor);
    bool setBlendColor = (mat->setColorFlags & 0b100) || (mat->otherModeValue & SOM_ALPHACOMPARE_THRESHOLD);
    setBlendColor = setBlendColor && color_to_packed32(state->lastBlendColor) != color_to_packed32(mat->blendColor);

    if(setBlendMode || setCC || setOtherMode || setTexture) {
      rdpq_sync_pipe();
    }

    if(setTexture)
    {
      state->lastTextureHashA = mat->textureA.textureHash;
      state->lastTextureHashB = mat->textureB.textureHash;
      rdpq_sync_load();

      rdpq_tex_multi_begin();
        set_texture(mat, TILE0, state->drawConf);
        set_texture(mat, TILE1, state->drawConf);
      rdpq_tex_multi_end();
    }

    if(setCC) {
      state->lastCC = mat->colorCombiner;
      rdpq_mode_combiner(mat->colorCombiner);
    }

    if(setBlendMode) {
      rdpq_mode_blender(mat->blendMode);
      state->lastBlendMode = mat->blendMode;
    }

    if(setPrimColor) {
      state->lastPrimColor = mat->primColor;
      rdpq_set_prim_color(mat->primColor);
    }

    if(setBlendColor) {
      state->lastBlendColor = mat->blendColor;
      rdpq_set_blend_color(mat->blendColor);
    }

    if(setEnvColor) {
      state->lastEnvColor = mat->envColor;
      rdpq_set_env_color(mat->envColor);
    }

    if(setOtherMode) {
      __rdpq_mode_change_som(mat->otherModeMask, mat->otherModeValue);
      state->lastOtherMode = mat->otherModeValue;
    }
  }
}

void t3d_model_free(T3DModel *model) {
  bool txtErased = false;

  if(model->userBlock) {
    rspq_block_free(model->userBlock);
  }

  for(uint32_t c = 0; c < model->chunkCount; c++)
  {
    char chunkType = model->chunkOffsets[c].type;
    if(chunkType == T3D_CHUNK_TYPE_MATERIAL) {
      T3DMaterial *mat = (T3DMaterial*)((char*)model + (model->chunkOffsets[c].offset & 0x00FFFFFF));
      if(mat->textureA.texture) {
        texture_cache_free(mat->textureA.textureHash);
        txtErased = true;
      }
      if(mat->textureB.texture) {
        texture_cache_free(mat->textureB.textureHash);
        txtErased = true;
      }
    }
    if(chunkType == T3D_CHUNK_TYPE_OBJECT) {
      T3DObject *obj = (T3DObject*)((char*)model + (model->chunkOffsets[c].offset & 0x00FFFFFF));
      if(obj->userBlock)rspq_block_free(obj->userBlock);
    }
  }
  free(model);
  if(txtErased) texture_cache_free_mem();
}

T3DChunkAnim *t3d_model_get_animation(const T3DModel *model, const char *name) {
  for(uint32_t i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == T3D_CHUNK_TYPE_ANIM) {
      uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
      T3DChunkAnim *anim = (T3DChunkAnim*)((char*)model + offset);
      if(strcmp(anim->name, name) == 0)return anim;
    }
  }
  return NULL;
}

T3DObject* t3d_model_get_object(const T3DModel *model, const char *name) {
  for(uint32_t i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == T3D_CHUNK_TYPE_OBJECT) {
      uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
      T3DObject *obj = (T3DObject*)((char*)model + offset);
      if(obj->name && strcmp(obj->name, name) == 0)return obj;
    }
  }
  return NULL;
}

void t3d_model_get_animations(const T3DModel *model, T3DChunkAnim **anims) {
  uint32_t count = 0;
  for(uint32_t i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == T3D_CHUNK_TYPE_ANIM) {
      uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
      anims[count++] = (T3DChunkAnim*)((char*)model + offset);
    }
  }
}

T3DMaterial *t3d_model_get_material(const T3DModel *model, const char *name) {
  for(uint32_t i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == T3D_CHUNK_TYPE_MATERIAL) {
      uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
      T3DMaterial *mat = (T3DMaterial*)((char*)model + offset);
      if(mat->name && strcmp(mat->name, name) == 0)return mat;
    }
  }
  return NULL;
}

bool t3d_model_iter_next(T3DModelIter *iter) {
  for(; iter->_idx < iter->_model->chunkCount; iter->_idx++) {
    if(iter->_model->chunkOffsets[iter->_idx].type == iter->_chunkType) {
      uint32_t offset = iter->_model->chunkOffsets[iter->_idx].offset & 0x00FFFFFF;
      iter->chunk = (char*)iter->_model + offset;
      iter->_idx++;
      return true;
    }
  }
  iter->chunk = NULL;
  return false;
}

// context for functions below, this avoids blowing up the stack-sie
static const T3DFrustum *ctxFrustum;
static const T3DBvhData *ctxData;
static uint32_t ctxBasePtr;

static void bvh_query_node(const T3DBvhNode *node) {
  int dataCount = node->value & 0b1111;
  int offset = (int16_t)node->value >> 4;

  if(dataCount == 0) {
    if(t3d_frustum_vs_aabb_s16(ctxFrustum, node->aabbMin, node->aabbMax)) {
      bvh_query_node(&node[offset]);
      bvh_query_node(&node[offset + 1]);
    }
    return;
  }

  int offsetEnd = offset + dataCount;
  while(offset < offsetEnd) {
    T3DObject* obj = (T3DObject*)(ctxBasePtr - (ctxData[offset++].objectPtr << 2));
    if(t3d_frustum_vs_aabb_s16(ctxFrustum, obj->aabbMin, obj->aabbMax)) {
      obj->isVisible = true;
    }
  }
}

void t3d_model_bvh_query_frustum(const T3DBvh *bvh, const T3DFrustum *frustum) {
  const T3DBvhData *data = (T3DBvhData*)&bvh->nodes[bvh->nodeCount]; // data starts right after nodes
  ctxFrustum = frustum;
  ctxData = data;
  ctxBasePtr = (uint32_t)(char*)bvh;
  bvh_query_node(bvh->nodes);
}
