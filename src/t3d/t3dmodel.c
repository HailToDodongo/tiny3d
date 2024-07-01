/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "t3dmodel.h"

static inline void* patch_pointer(void *ptr, uint32_t offset) {
  return (void*)(offset + (int32_t)ptr);
}

typedef struct {
  uint32_t hash;
  sprite_t *texture;
  uint32_t count;
} T3DTextureEntry;

static uint32_t textureCacheSize = 0;
static T3DTextureEntry *textureCache = NULL;

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
      //debugf("Free Texture: %08lX, count=%ld\n", hash, textureCache[i].count);
      if(textureCache[i].count == 0) {
        sprite_free(textureCache[i].texture);
        textureCache[i].hash = 0;
      }
      return;
    }
  }
}

static void set_texture(T3DMaterial *mat, rdpq_tile_t tile, T3DModelDrawConf *conf)
{
  if(mat->texPath || mat->texReference)
  {
    //debugf("Load Texture: %s (%08lX)\n", mat->texPath, mat->textureHash);
    if(mat->texPath && !mat->texture) {
      mat->texture = texture_cache_get(mat->textureHash);
      if(mat->texture == NULL) {
        debugf("Not in cache, load %s (%08lX)\n", mat->texPath, mat->textureHash);
        mat->texture = sprite_load(mat->texPath);
        //const char* formatName = tex_format_name(sprite_get_format(mat->texture));
        //debugf(" -> %s\n", formatName);
        texture_cache_add(mat->textureHash, mat->texture);
      }
    }

    rdpq_texparms_t tex = (rdpq_texparms_t){};
    tex.s.mirror = mat->s.mirror;
    tex.s.repeats = mat->s.clamp ? 1 : REPEAT_INFINITE;
    tex.s.scale_log = (int)mat->s.shift;

    tex.t.mirror = mat->t.mirror;
    tex.t.repeats = mat->t.clamp ? 1 : REPEAT_INFINITE;
    tex.t.scale_log = (int)mat->t.shift;

    if(conf->tileCb) {
      conf->tileCb(conf->userData, &tex, tile);
    }

    // @TODO: don't upload texture if only the tile settings differ
    if(mat->texReference) {
      if(conf->dynTextureCb)conf->dynTextureCb(conf->userData, mat, &tex, tile);
    } else {
      rdpq_sync_tile();
      rdpq_sprite_upload(tile, mat->texture, &tex);
    }
  }
}

T3DModel *t3d_model_load(const char *path) {
  int size = 0;
  T3DModel* model = asset_load(path, &size);
  int32_t ptrOffset = (int32_t)(void*)model;

  void* basePtrVertices = (char*)model + (model->chunkOffsets[model->chunkIdxVertices].offset & 0xFFFFFF);
  void* basePtrIndices = (char*)model + (model->chunkOffsets[model->chunkIdxIndices].offset & 0xFFFFFF);
  model->stringTablePtr = patch_pointer(model->stringTablePtr, ptrOffset);

  for(uint32_t i = 0; i < model->chunkCount; i++)
  {
    char chunkType = model->chunkOffsets[i].type;
    uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
    //debugf("Chunk[%d] '%c': %lx\n", i, chunkType, offset);

    if(chunkType == 'O') {
      T3DObject *obj = (T3DObject*)((char*)model + offset);
      if(obj->name != NULL) {
        obj->name = patch_pointer(obj->name, (uint32_t)model->stringTablePtr);
      }

      uint32_t matIdxA = model->chunkIdxMaterials + (uint32_t)obj->materialA;
      uint32_t matIdxB = model->chunkIdxMaterials + (uint32_t)obj->materialB;
      obj->materialA = (T3DMaterial*)((char*)model + (model->chunkOffsets[matIdxA].offset & 0xFFFFFF));
      obj->materialB = (T3DMaterial*)((char*)model + (model->chunkOffsets[matIdxB].offset & 0xFFFFFF));

      if(obj->materialA->texPath) {
        obj->materialA->texPath += (uint32_t)model->stringTablePtr;
      }
      if(obj->materialB->texPath) {
        obj->materialB->texPath += (uint32_t)model->stringTablePtr;
      }

      for(uint32_t j = 0; j < obj->numParts; j++) {
        T3DObjectPart *part = &obj->parts[j];
        part->indices = patch_pointer(part->indices, (uint32_t)basePtrIndices);
        part->vert = patch_pointer(part->vert, (uint32_t)basePtrVertices);
      }
    }

    if(chunkType == 'S') {
      T3DChunkSkeleton *skel = (T3DChunkSkeleton*)((char*)model + offset);
      for(int j = 0; j < skel->boneCount; j++) {
        T3DChunkBone *bone = &skel->bones[j];
        bone->name = patch_pointer(bone->name, (uint32_t)model->stringTablePtr);
      }
    }

    if(chunkType == 'A') {
      T3DChunkAnim *anim = (T3DChunkAnim*)((char*)model + offset);
      anim->name = patch_pointer(anim->name, (uint32_t)model->stringTablePtr);
      anim->filePath = patch_pointer(anim->filePath, (uint32_t)model->stringTablePtr);
    }
  }

  data_cache_hit_writeback_invalidate(model, size);
  return model;
}

void t3d_model_draw_custom(const T3DModel* model, T3DModelDrawConf conf)
{
  // keeps track of various states to not emit useless RSP commands
  // all defaults are chosen to cause a call in the first iteration
  uint32_t lastTextureHashA = 0;
  uint32_t lastTextureHashB = 0;
  uint8_t lastFogMode = 0xFF;
  uint8_t lastAlphaMode = 0xFF;
  uint8_t lastTexFilter = 0xFF;
  uint32_t lastRenderFlags = 0;
  uint64_t lastCC = 0;
  color_t lastPrimColor = (color_t){0,0,0,0};
  bool hadMatrixPush = false;
  uint8_t lastVertFXFunc = T3D_VERTEX_FX_NONE;
  uint16_t lastUvGenParams[2] = {0,0};
  uint8_t lastZMode = 0xFF;

  uint64_t lastOtherMode = 0;

  for(uint32_t c = 0; c < model->chunkCount; c++) {
    char chunkType = model->chunkOffsets[c].type;
    if(chunkType != 'O')break;

    uint32_t offset = model->chunkOffsets[c].offset & 0x00FFFFFF;
    const T3DObject *obj = (T3DObject*)((char*)model + offset);

    // check a user-provided object filter
    if(conf.filterCb && !conf.filterCb(conf.userData, obj)) {
      continue;
    }

    T3DMaterial *matMain = obj->materialA;
    T3DMaterial *matSecond = obj->materialB;

    // first change t3d settings, this avoids an overlay-switch.
    // settings here also influence how 't3d_vert_load' will load vertices
    if(matMain)
    {
      if(obj->materialA->renderFlags != lastRenderFlags) {
        t3d_state_set_drawflags(obj->materialA->renderFlags);
        lastRenderFlags = obj->materialA->renderFlags;
      }

      if(matMain->fogMode != T3D_FOG_MODE_DEFAULT && matMain->fogMode != lastFogMode) {
        lastFogMode = matMain->fogMode;
        t3d_fog_set_enabled(matMain->fogMode == T3D_FOG_MODE_ACTIVE);
      }
    }

    bool hadMaterial = false;
    for(uint32_t p = 0; p < obj->numParts; p++)
    {
      const T3DObjectPart *part = &obj->parts[p];

      if(conf.matrices) {
        if(part->matrixIdx != 0xFFFF) {
          if(!hadMatrixPush) {
            t3d_matrix_push(&conf.matrices[part->matrixIdx]);
          } else {
            t3d_matrix_set(&conf.matrices[part->matrixIdx], true);
          }
          hadMatrixPush = true;
        } else {
          if(hadMatrixPush) {
            t3d_matrix_pop(1);
            hadMatrixPush = false;
          }
        }
      }

      if(matMain) {
        if(lastVertFXFunc != matMain->vertexFxFunc || (
          matMain->vertexFxFunc && (lastUvGenParams[0] != matMain->texWidth || lastUvGenParams[1] != matMain->texHeight)
        )) {
          lastVertFXFunc = matMain->vertexFxFunc;
          lastUvGenParams[0] = matMain->texWidth;
          lastUvGenParams[1] = matMain->texHeight;
          t3d_state_set_vertex_fx(lastVertFXFunc, (int16_t)matMain->texWidth, (int16_t)matMain->texHeight);
        }
      }

      // load vertices, this will already do T&L (so matrices/fog/lighting must be set before)
      t3d_vert_load(part->vert, part->vertDestOffset, part->vertLoadCount);
      //debugf("Load Vertices[%d]: %d, %d | bone: %d\n", p, part->vertDestOffset, part->vertLoadCount, part->matrixIdx);
      if(part->numIndices == 0)continue; // partial-load, last chunk of a sequence will both indices & material data

      bool hadPipeSync = false;

      // now apply rdpq settings, these are independent of the t3d state
      // and only need to happen before a `t3d_tri_draw` call
      if(!hadMaterial && matMain && matMain->colorCombiner)
      {
        hadMaterial = true;
        bool hadTexLoad = false;

        //debugf("TexA: %08lX (ref: %08lX), TexB: %08lX (ref: %08lX)\n",
        //  matMain->textureHash, matMain->texReference, matSecond->textureHash, matSecond->texReference);

        if(lastTextureHashA != matMain->textureHash || lastTextureHashB != matSecond->textureHash) {
          lastTextureHashA = matMain->textureHash;
          lastTextureHashB = matSecond->textureHash;
          hadTexLoad = true;

          rdpq_sync_pipe();
          rdpq_sync_load();
          hadPipeSync = true;

          rdpq_tex_multi_begin();
            set_texture(matMain, TILE0, &conf);
            set_texture(matSecond, TILE1, &conf);
          rdpq_tex_multi_end();
        }

        if(matMain->colorCombiner != lastCC)
        {
          lastCC = matMain->colorCombiner;
          if(!hadPipeSync) {
            rdpq_sync_pipe();
            hadPipeSync = true;
          }
          rdpq_mode_combiner(obj->materialA->colorCombiner);
        }

        uint8_t zMode = matMain->zModeColorMask >> 4;
        if(zMode != lastZMode) {
          if(!hadPipeSync) {
            rdpq_sync_pipe();
            hadPipeSync = true;
          }
          rdpq_change_other_modes_raw(zMode << SOM_ZMODE_SHIFT, SOM_ZMODE_MASK);
        }

        uint8_t alphaMode = matMain->filterAlphaMode & 0xF;
        uint8_t texFilter = matMain->filterAlphaMode >> 4;

        if(texFilter != lastTexFilter) {
          if(!hadPipeSync) {
            rdpq_sync_pipe();
            hadPipeSync = true;
          }
          rdpq_mode_filter(texFilter);
          lastTexFilter = texFilter;
        }

        if(alphaMode != lastAlphaMode || hadTexLoad) // @TODO: why 'hadTexLoad'?
        {
          if(!hadPipeSync) {
            rdpq_sync_pipe();
            hadPipeSync = true;
          }

          switch (alphaMode) {
            case T3D_ALPHA_MODE_TRANSP:
              rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
              break;
            case T3D_ALPHA_MODE_CUTOUT:
            case T3D_ALPHA_MODE_OPAQUE:
              rdpq_mode_blender(0);
            break;
          }

          lastAlphaMode = alphaMode;
        }

        bool setPrimColor  = matMain->zModeColorMask & 0b001;
        bool setEnvColor   = matMain->zModeColorMask & 0b010;
        bool setBlendColor = matMain->zModeColorMask & 0b100;

        // Apply primary color if needed, no sync needed here
        if(setPrimColor) {
          if(color_to_packed32(lastPrimColor) != color_to_packed32(matMain->primColor)) {
            rdpq_set_prim_color(matMain->primColor); // @TODO: have t3d version of this
            lastPrimColor = matMain->primColor;
          }
        }

        if(lastOtherMode != matMain->otherModeValue) {
          if(!hadPipeSync) {
            rdpq_sync_pipe();
            hadPipeSync = true;
          }
          __rdpq_mode_change_som(matMain->otherModeMask, matMain->otherModeValue);
          lastOtherMode = matMain->otherModeValue;

          if(matMain->otherModeValue & SOM_ALPHACOMPARE_THRESHOLD) {
            rdpq_set_blend_color(matMain->blendColor); // alpha used as threshold
            setBlendColor = false;
          }
        }

        if(setEnvColor)rdpq_set_env_color(matMain->envColor);
        if(setBlendColor)rdpq_set_blend_color(matMain->blendColor);
      }

      // now draw all triangles of the part
      for(int i = 0; i < part->numIndices; i+=3) {
        t3d_tri_draw(part->indices[i], part->indices[i+1], part->indices[i+2]);
      }

      // Sync, waits for any triangles in flight. This is necessary since the rdpq-api is not
      // aware of this and could corrupt the RDP buffer. 't3d_vert_load' may also overwrite the source buffer too.
      t3d_tri_sync();

      // At this point the RDP may already process triangles.
      // In the next iteration we may therefore need to sync when changing any RDP states
    }
  }

  if(hadMatrixPush)t3d_matrix_pop(1);
  if(lastVertFXFunc != T3D_VERTEX_FX_NONE)t3d_state_set_vertex_fx(T3D_VERTEX_FX_NONE, 0, 0);
}

void t3d_model_draw_manual(const T3DModel* model, T3DModelManualCb cb, void* userData, const T3DMat4FP *boneMatrices)
{
  bool hadMatrixPush = false;
  uint32_t objectIdx = 0;

  for(uint32_t c = 0; c < model->chunkCount; c++) {
    char chunkType = model->chunkOffsets[c].type;
    if(chunkType != 'O')break;

    uint32_t offset = model->chunkOffsets[c].offset & 0x00FFFFFF;
    const T3DObject *obj = (T3DObject*)((char*)model + offset);

    for(uint32_t p = 0; p < obj->numParts; p++)
    {
      const T3DObjectPart *part = &obj->parts[p];
      if(cb && !cb(userData, obj, part, objectIdx++, p))continue;

      if(boneMatrices) {
        if(part->matrixIdx != 0xFFFF) {
          if(!hadMatrixPush) {
            t3d_matrix_push(&boneMatrices[part->matrixIdx]);
          } else {
            t3d_matrix_set(&boneMatrices[part->matrixIdx], true);
          }
          hadMatrixPush = true;
        } else {
          if(hadMatrixPush) {
            t3d_matrix_pop(1);
            hadMatrixPush = false;
          }
        }
      }

      t3d_vert_load(part->vert, part->vertDestOffset, part->vertLoadCount);
      for(int i = 0; i < part->numIndices; i+=3) {
        t3d_tri_draw(part->indices[i], part->indices[i+1], part->indices[i+2]);
      }
      t3d_tri_sync();
    }
  }

  if(hadMatrixPush)t3d_matrix_pop(1);
}

void t3d_model_free(T3DModel *model) {
  for(uint32_t c = 0; c < model->chunkCount; c++) {
    char chunkType = model->chunkOffsets[c].type;
    if(chunkType != 'M')break;
    T3DMaterial *mat = (T3DMaterial*)((char*)model + (model->chunkOffsets[c].offset & 0x00FFFFFF));
    if(mat->texture) {
      texture_cache_free(mat->textureHash);
    }
  }
  free(model);
}

T3DChunkAnim *t3d_model_get_animation(const T3DModel *model, const char *name) {
  for(uint32_t i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == 'A') {
      uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
      T3DChunkAnim *anim = (T3DChunkAnim*)((char*)model + offset);
      if(strcmp(anim->name, name) == 0)return anim;
    }
  }
  return NULL;
}

T3DObject* t3d_model_get_object(const T3DModel *model, const char *name) {
  for(uint32_t i = 0; i < model->chunkCount; i++) {
    if(model->chunkOffsets[i].type == 'O') {
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
    if(model->chunkOffsets[i].type == 'A') {
      uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
      anims[count++] = (T3DChunkAnim*)((char*)model + offset);
    }
  }
}

