/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "t3dskeleton.h"
#include <malloc.h>

static_assert(sizeof(T3DBone) == 96, "T3DBone should be exactly 6 cache-lines");

// Same as 't3d_mat4_from_srt', but for a 4x3 matrix
static void mat4x3_from_srt(T3DMat4x3 *mat, const float scale[3], const float quat[4], const float translate[3])
{
  // read all inputs first, since 'mat' could alias them
  float scaleX = scale[0];
  float scaleY = scale[1];
  float scaleZ = scale[2];
  float posX = translate[0];
  float posY = translate[1];
  float posZ = translate[2];

  float qxx = quat[0] * quat[0];
  float qyy = quat[1] * quat[1];
  float qzz = quat[2] * quat[2];
  float qxz = quat[0] * quat[2];
  float qxy = quat[0] * quat[1];
  float qyz = quat[1] * quat[2];
  float qwx = quat[3] * quat[0];
  float qwy = quat[3] * quat[1];
  float qwz = quat[3] * quat[2];

  *mat = (T3DMat4x3){{
    {(1.0f - 2.0f * (qyy + qzz)) * scaleX, (2.0f * (qxy + qwz)) * scaleX,        (2.0f * (qxz - qwy)) * scaleX},
    {(2.0f * (qxy - qwz)) * scaleY,        (1.0f - 2.0f * (qxx + qzz)) * scaleY, (2.0f * (qyz + qwx)) * scaleY},
    {(2.0f * (qxz + qwy)) * scaleZ,        (2.0f * (qyz - qwx)) * scaleZ,        (1.0f - 2.0f * (qxx + qyy)) * scaleZ},
    {posX,                                 posY,                                 posZ}
  }};
}

// Same as 't3d_mat4_mul', but for affine 4x3 matrices
static void mat4x3_mul(T3DMat4x3 *matRes, const T3DMat4x3 *matA, const T3DMat4x3 *matB)
{
  for(uint32_t i=0; i<3; i++) {
    for(uint32_t j=0; j<3; j++) {
      matRes->m[j][i] = matA->m[0][i] * matB->m[j][0] +
                        matA->m[1][i] * matB->m[j][1] +
                        matA->m[2][i] * matB->m[j][2];
    }
    matRes->m[3][i] = matA->m[0][i] * matB->m[3][0] +
                      matA->m[1][i] * matB->m[3][1] +
                      matA->m[2][i] * matB->m[3][2] +
                      matA->m[3][i];
  }
}

// Same as 't3d_mat4_to_fixed_3x4', but from a 4x3 matrix
static void mat4x3_to_fixed(T3DMat4FP *matOut, const T3DMat4x3 *matIn) {
  for(uint32_t y=0; y<4; ++y) {
    uint32_t fixed0 = T3D_F32_TO_FIXED(matIn->m[y][0]);
    uint32_t fixed1 = T3D_F32_TO_FIXED(matIn->m[y][1]);
    uint32_t fixed2 = T3D_F32_TO_FIXED(matIn->m[y][2]);

    // prepare 64bit values, this creates less writes to memory later
    uint64_t I = (fixed0 & 0xFFFF0000) | (fixed1 >> 16);
    I <<= 32; // needs to be separate, otherwise -Os generates wrong code
    I |= (fixed2 & 0xFFFF0000);

    // puts a '1' into the last value of the matrix
    I |= (y+1) >> 2;

    uint64_t F = (fixed0 << 16) | (fixed1 & 0xFFFF);
    F <<= 32; // needs to be separate, otherwise -Os generates wrong code
    F |= (fixed2 << 16);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
      *(uint64_t*)matOut->m[y].i = I; // guaranteed to be 64-bit aligned
      *(uint64_t*)matOut->m[y].f = F;
    #pragma GCC diagnostic pop
  }
}

T3DSkeleton t3d_skeleton_create_buffered(const T3DModel *model, int bufferCount) {
  const T3DChunkSkeleton *skelRef = t3d_model_get_skeleton(model);
  assert(skelRef != NULL);

  T3DSkeleton skel = (T3DSkeleton){
    .bones = memalign(16, sizeof(T3DBone) * skelRef->boneCount),
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
    .bones = memalign(16, sizeof(T3DBone) * skel->skeletonRef->boneCount),
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
    skeleton->bones[i].parentIdx = boneDef->parentIdx;
    skeleton->bones[i].depth = boneDef->depth;
  }
}

void t3d_skeleton_blend(const T3DSkeleton *skelRes, const T3DSkeleton *skelA, const T3DSkeleton *skelB, float factor) {
  for(int i = 0; i < skelRes->skeletonRef->boneCount; i++) {
    T3DBone *boneRes = &skelRes->bones[i];
    T3DBone *boneA = &skelA->bones[i];
    T3DBone *boneB = &skelB->bones[i];

    boneRes->hasChanged = true;
    t3d_quat_nlerp(&boneRes->rotation, &boneA->rotation, &boneB->rotation, factor);
    t3d_vec3_lerp(&boneRes->position, &boneA->position, &boneB->position, factor);
    t3d_vec3_lerp(&boneRes->scale, &boneA->scale, &boneB->scale, factor);
  }
}

void t3d_skeleton_update(T3DSkeleton *skeleton)
{
  int updateLevel = -1;
  uint32_t forceUpdate = 0;

  T3DMat4FP* matStackFP = nullptr;
  T3DMat4x3 tmp __attribute__((uninitialized));

  for(int i = 0; i < skeleton->skeletonRef->boneCount; i++)
  {
    T3DBone *bone = &skeleton->bones[i];

    if(forceUpdate && bone->depth <= updateLevel) {
      forceUpdate = false;
      updateLevel = -1;
    }

    const bool hasChanged = bone->hasChanged | forceUpdate;
    if(hasChanged)
    {
      // if a bone changed we need to also update any children.
      // To do so, update all following bones until we hit one that has the same depth as the changed bone.
      if(!forceUpdate)updateLevel = bone->depth;
      forceUpdate = 1;
      
      // only cycle through matrices if at least one bone changes.
      // this avoids flickering at the end of an animation, since it would cycle through the last X frames otherwise.
      if(matStackFP == nullptr)
      {
        skeleton->currentBufferIdx = (skeleton->currentBufferIdx + 1) % skeleton->bufferCount;
        matStackFP = &skeleton->boneMatricesFP[skeleton->skeletonRef->boneCount * skeleton->currentBufferIdx];
      }

      if(bone->parentIdx != 0xFFFF) {
        mat4x3_from_srt(&tmp, bone->scale.v, bone->rotation.v, bone->position.v);
        mat4x3_mul(&bone->matrix, &skeleton->bones[bone->parentIdx].matrix, &tmp);
      } else {
        mat4x3_from_srt(&bone->matrix, bone->scale.v, bone->rotation.v, bone->position.v);
      }

      mat4x3_to_fixed(&matStackFP[i], &bone->matrix);

      // if a bone has changed, we need to force updating it until it reached all buffers.
      // otherwise once the updating stops, and we cycle through buffers still, it would flicker.
      // counting up here is also safe when this flag is set to 1 or 'true' externally (e.g.: in t3d_anim_update)
      if(skeleton->bones[i].hasChanged++ == skeleton->bufferCount) {
        skeleton->bones[i].hasChanged = 0;
      }
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

