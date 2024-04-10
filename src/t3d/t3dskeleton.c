/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "t3dskeleton.h"

T3DSkeleton t3d_skeleton_create(const T3DModel *model) {
  const T3DChunkSkeleton *skelRef = t3d_model_get_skeleton(model);
  assert(skelRef != NULL);

  T3DSkeleton skel = (T3DSkeleton){
    .bones = malloc(sizeof(T3DBone) * skelRef->boneCount),
    .boneMatricesFP = malloc_uncached(sizeof(T3DMat4FP) * skelRef->boneCount),
    .skeletonRef = skelRef
  };

  t3d_skeleton_reset(&skel);
  for(int i = 0; i < skelRef->boneCount; i++) {
    t3d_mat4_identity(&skel.bones[i].matrix);
    t3d_mat4fp_identity(&skel.boneMatricesFP[i]);
  }
  return skel;
}

void t3d_skeleton_reset(T3DSkeleton *skeleton) {
  for(int i = 0; i < skeleton->skeletonRef->boneCount; i++) {
    t3d_quat_identity(&skeleton->bones[i].rotation);
    skeleton->bones[i].position = (T3DVec3){{0, 0, 0}};
    skeleton->bones[i].scale = (T3DVec3){{1, 1, 1}};
    skeleton->bones[i].hasChanged = true;
  }
}

void t3d_skeleton_update(const T3DSkeleton *skeleton)
{
  T3DMat4 tmpMat;
  int updateLevel = -1;
  bool forceUpdate = false;

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

      t3d_mat4_from_srt(&tmpMat, bone->scale.v, bone->rotation.v, bone->position.v);

      const T3DMat4 *baseMatrix = &boneDef->transform;

      if(boneDef->parentIdx != 0xFFFF) {
        T3DMat4 tmpMat2;
        t3d_mat4_mul(&tmpMat2, baseMatrix, &tmpMat);
        t3d_mat4_mul(&bone->matrix, &skeleton->bones[boneDef->parentIdx].matrix, &tmpMat2);
      } else {
        t3d_mat4_mul(&bone->matrix, baseMatrix, &tmpMat);
      }

      t3d_mat4_to_fixed(&skeleton->boneMatricesFP[i], &bone->matrix);
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
    free(skeleton->boneMatricesFP);
    skeleton->boneMatricesFP = NULL;
  }
  skeleton->skeletonRef = NULL;
}

