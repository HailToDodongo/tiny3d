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
  for(int i = 0; i < textureCacheSize; i++) {
    if(textureCache[i].hash == hash) {
      textureCache[i].count++;
      return textureCache[i].texture;
    }
  }
  return NULL;
}

static void texture_cache_add(uint32_t hash, sprite_t *texture) {
  T3DTextureEntry *cacheEntry = NULL;
  for(int i = 0; i < textureCacheSize; i++) {
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
  for(int i = 0; i < textureCacheSize; i++) {
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
  if(mat->texPath)
  {
    if(!mat->texture) {
      debugf("Load Texture: %s (%08lX)\n", mat->texPath, mat->textureHash);
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
    rdpq_sprite_upload(tile, mat->texture, &tex);
  }
}

T3DModel *t3d_model_load(const void *path) {
  int size = 0;
  T3DModel* model = asset_load(path, &size);
  int32_t ptrOffset = (int32_t)(void*)model;

  void* basePtrVertices = (void*)model + (model->chunkOffsets[model->chunkIdxVertices].offset & 0xFFFFFF);
  void* basePtrIndices = (void*)model + (model->chunkOffsets[model->chunkIdxIndices].offset & 0xFFFFFF);
  model->stringTablePtr = patch_pointer(model->stringTablePtr, ptrOffset);

  for(int i = 0; i < model->chunkCount; i++)
  {
    char chunkType = model->chunkOffsets[i].type;
    uint32_t offset = model->chunkOffsets[i].offset & 0x00FFFFFF;
    //debugf("Chunk[%d] '%c': %lx\n", i, chunkType, offset);

    if(chunkType == 'O') {
      T3DObject *obj = (void*)model + offset;

      uint32_t matIdxA = model->chunkIdxMaterials + (uint32_t)obj->materialA;
      uint32_t matIdxB = model->chunkIdxMaterials + (uint32_t)obj->materialB;
      obj->materialA = (void*)model + (model->chunkOffsets[matIdxA].offset & 0xFFFFFF);
      obj->materialB = (void*)model + (model->chunkOffsets[matIdxB].offset & 0xFFFFFF);

      if(obj->materialA->texPath) {
        obj->materialA->texPath += (uint32_t)model->stringTablePtr;
      }
      if(obj->materialB->texPath) {
        obj->materialB->texPath += (uint32_t)model->stringTablePtr;
      }

      for(int j = 0; j < obj->numParts; j++) {
        T3DObjectPart *part = &obj->parts[j];
        part->indices = patch_pointer(part->indices, (uint32_t)basePtrIndices);
        part->vert = patch_pointer(part->vert, (uint32_t)basePtrVertices);
      }
    }
  }

  data_cache_hit_writeback_invalidate(model, size);
  return model;
}

void t3d_model_draw_custom(const T3DModel* model, T3DModelDrawConf conf)
{
  uint32_t lastTextureHashA = 0;
  uint32_t lastTextureHashB = 0;

  for(int c = 0; c < model->chunkCount; c++) {
    char chunkType = model->chunkOffsets[c].type;
    if(chunkType != 'O')break;

    uint32_t offset = model->chunkOffsets[c].offset & 0x00FFFFFF;
    const T3DObject *obj = (void*)model + offset;

    for (int p = 0; p < obj->numParts; p++) {
      const T3DObjectPart *part = &obj->parts[p];
      t3d_vert_load(part->vert, 0, part->vertLoadCount);

      T3DMaterial *matMain = obj->materialA;
      T3DMaterial *matSecond = obj->materialB;
      if(p == 0 && matMain && matMain->colorCombiner)
      {
        t3d_state_set_drawflags(obj->materialA->renderFlags);

        if(lastTextureHashA != matMain->textureHash || lastTextureHashB != matSecond->textureHash) {
          lastTextureHashA = matMain->textureHash;
          lastTextureHashB = matSecond->textureHash;

          rdpq_sync_tile();
          rdpq_sync_pipe();

          rdpq_tex_multi_begin();
            set_texture(matMain, TILE0, &conf);
            set_texture(matSecond, TILE1, &conf);
          rdpq_tex_multi_end();
        }

        rdpq_sync_pipe();
        rdpq_mode_combiner(obj->materialA->colorCombiner);
      }

      for(int i = 0; i < part->numIndices; i+=3) {
        t3d_tri_draw(part->indices[i], part->indices[i+1], part->indices[i+2]);
      }
    }
  }
}

void t3d_model_free(T3DModel *model) {
  for(int c = 0; c < model->chunkCount; c++) {
    char chunkType = model->chunkOffsets[c].type;
    if(chunkType != 'M')break;
    T3DMaterial *mat = (void*)model + (model->chunkOffsets[c].offset & 0x00FFFFFF);
    if(mat->texture) {
      texture_cache_free(mat->textureHash);
    }
  }
  free(model);
}

