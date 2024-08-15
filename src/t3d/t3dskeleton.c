/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "t3dskeleton.h"

T3DSkeleton t3d_skeleton_create_buffered(const T3DModel *model, int bufferCount) {
  const T3DChunkSkeleton *skelRef = t3d_model_get_skeleton(model);
  assert(skelRef != NULL);

  T3DSkeleton skel = (T3DSkeleton){
    .bones = malloc(sizeof(T3DBone) * skelRef->boneCount),
    .boneMatricesFP = malloc_uncached(sizeof(T3DMat4FP) * skelRef->boneCount * bufferCount),
    .skeletonRef = skelRef,
    .bufferCount = bufferCount,
    .currentBufferIdx = 0,
  };

  t3d_skeleton_reset(&skel);
  return skel;
}

T3DSkeleton t3d_skeleton_clone(const T3DSkeleton *skel, bool useMatrices) {
  T3DSkeleton result = {
    .bones = malloc(sizeof(T3DBone) * skel->skeletonRef->boneCount),
    .boneMatricesFP = NULL,
    .skeletonRef = skel->skeletonRef,
  };
  memcpy(result.bones, skel->bones, sizeof(T3DBone) * skel->skeletonRef->boneCount);

  if(useMatrices) {
    size_t copySize = sizeof(T3DMat4FP) * skel->skeletonRef->boneCount * skel->bufferCount;
    result.boneMatricesFP = malloc_uncached(copySize);
    memcpy(result.boneMatricesFP, skel->boneMatricesFP, copySize);
  }
  return result;
}

void t3d_skeleton_reset(T3DSkeleton *skeleton) {
  for(int i = 0; i < skeleton->skeletonRef->boneCount; i++) {
    const T3DChunkBone *boneDef = &skeleton->skeletonRef->bones[i];
    memcpy(skeleton->bones[i].scale.v, boneDef->scale.v,
      sizeof(T3DVec3) + sizeof(T3DQuat) + sizeof(T3DVec3) // copy all 3 vectors (SRT) at once
    );
    skeleton->bones[i].hasChanged = true;
  }
}

void t3d_skeleton_blend(const T3DSkeleton *skelRes, const T3DSkeleton *skelA, const T3DSkeleton *skelB, float factor) {
  for(int i = 0; i < skelRes->skeletonRef->boneCount; i++) {
    T3DBone *boneRes = &skelRes->bones[i];
    T3DBone *boneA = &skelA->bones[i];
    T3DBone *boneB = &skelB->bones[i];

    t3d_quat_nlerp(&boneRes->rotation, &boneA->rotation, &boneB->rotation, factor);
    t3d_vec3_lerp(&boneRes->position, &boneA->position, &boneB->position, factor);
    t3d_vec3_lerp(&boneRes->scale, &boneA->scale, &boneB->scale, factor);
  }
}

void t3d_skeleton_update(T3DSkeleton *skeleton)
{
  int updateLevel = -1;
  bool forceUpdate = false;

  skeleton->currentBufferIdx = (skeleton->currentBufferIdx + 1) % skeleton->bufferCount;
  T3DMat4FP* matStackFP = &skeleton->boneMatricesFP[skeleton->skeletonRef->boneCount * skeleton->currentBufferIdx];

  for(int i = 0; i < skeleton->skeletonRef->boneCount; i++)
  {
    T3DBone *bone = &skeleton->bones[i];
    const T3DChunkBone *boneDef = &skeleton->skeletonRef->bones[i];

    if(forceUpdate && boneDef->depth <= updateLevel) {
      forceUpdate = false;
      updateLevel = -1;
    }

    if(bone->hasChanged || forceUpdate)
    {
      // if a bone changed we need to also update any children.
      // To do so, update all following bones until we hit one that has the same depth as the changed bone.
      if(!forceUpdate)updateLevel = boneDef->depth;
      forceUpdate = true;

      if(boneDef->parentIdx != 0xFFFF) {
        T3DMat4 tmp;
        t3d_mat4_from_srt(&tmp, bone->scale.v, bone->rotation.v, bone->position.v);
        t3d_mat4_mul(&bone->matrix, &skeleton->bones[boneDef->parentIdx].matrix, &tmp);
      } else {
        t3d_mat4_from_srt(&bone->matrix, bone->scale.v, bone->rotation.v, bone->position.v);
      }

      t3d_mat4_to_fixed(&matStackFP[i], &bone->matrix);
      skeleton->bones[i].hasChanged = false;
    }
  }
}

int t3d_skeleton_find_bone(T3DSkeleton *skeleton, const char *name) {
  for(int i = 0; i < skeleton->skeletonRef->boneCount; i++) {
    if(strcmp(skeleton->skeletonRef->bones[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}

void t3d_skeleton_destroy(T3DSkeleton *skeleton) {
  if(skeleton->bones != NULL) {
    free(skeleton->bones);
    skeleton->bones = NULL;
  }
  if(skeleton->boneMatricesFP != NULL) {
    free_uncached(skeleton->boneMatricesFP);
    skeleton->boneMatricesFP = NULL;
  }
  skeleton->skeletonRef = NULL;
}

