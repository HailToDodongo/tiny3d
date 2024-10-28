/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#ifndef TINY3D_T3DSKELETON_H
#define TINY3D_T3DSKELETON_H

#include "t3dmodel.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Bone instance, part of a skeleton.
 * 'matrix' will get updated by the skeleton when calling t3d_skeleton_update,
 * if 'hasChanged' is set to true.
 */
typedef struct {
  T3DMat4 matrix;
  T3DVec3 scale;
  T3DQuat rotation;
  T3DVec3 position;
  int32_t hasChanged;
} T3DBone;

/**
 * Skeleton instance, can be constructed from a model's skeleton definition.
 * This is used to draw skinned models.
 */
typedef struct {
  T3DBone* bones;
  T3DMat4FP* boneMatricesFP; // fixed point matrix, used for rendering
  uint8_t bufferCount; // number of matrices buffers
  uint8_t currentBufferIdx;
  const T3DChunkSkeleton* skeletonRef; // reference to the model, defines skeleton structure
} T3DSkeleton;

/**
 * Creates a skeleton instance from a model's skeleton definition.
 * It will internally reserve multiple matrix stacks to allow for buffering.
 * This can be used to update a skeleton while the last frame is still being rendered.
 * Note that only the fixed-point matrices are buffered, the bone data itself is not.
 *
 * @param model The model to create the skeleton from
 * @param bufferCount number of buffers, should match the frame-buffer count
 * @return The created skeleton
 */
T3DSkeleton t3d_skeleton_create_buffered(const T3DModel *model, int bufferCount);

/**
 * Creates a skeleton instance from a model's skeleton definition
 * @param model The model to create the skeleton from
 * @return The created skeleton
 */
static inline T3DSkeleton t3d_skeleton_create(const T3DModel *model) {
  return t3d_skeleton_create_buffered(model, 1);
}

/**
 * Sets the given skeleton up for the next draw call.
 * This has to be used for buffered skeletons, but is a no-op for single-buffer skeletons.
 * @param skel skeleton to use in the next draw
 */
static inline void t3d_skeleton_use(const T3DSkeleton *skel) {
  if(skel->bufferCount > 1) {
    void* mat = skel->boneMatricesFP + skel->currentBufferIdx * skel->skeletonRef->boneCount;
    t3d_segment_set(T3D_SEGMENT_SKELETON, mat);
  }
}

/**
 * Clones a skeleton instance.
 * This can also be used to create optimized skeletons for blending animations.
 * @param skel Skeleton to clone
 * @param useMatrices If false, no matrices will be allocated, this is useful for blending animations
 * @return Cloned skeleton
 */
T3DSkeleton t3d_skeleton_clone(const T3DSkeleton *skel, bool useMatrices);

/**
 * Resets a skeleton to its initial state (resting pose).
 * This can be useful when switching between animations.
 * Note: To recalculate the bone matrices too, call 't3d_skeleton_update' afterwards.
 * @param skeleton The skeleton to reset
 */
void t3d_skeleton_reset(T3DSkeleton *skeleton);

/**
 * Blends two skeletons together.
 * Note: it is safe to use the same skeleton as an input and output parameter.
 * @param skelRes Resulting skeleton
 * @param skelA First skeleton
 * @param skelB Second skeleton
 * @param factor Blend factor (0.0-1.0), you are allowed to go beyond these values to "overdrive" animations
 */
void t3d_skeleton_blend(const T3DSkeleton *skelRes, const T3DSkeleton *skelA, const T3DSkeleton *skelB, float factor);

/**
 * Updates the skeleton's bone matrices if data has changed.
 * Call this after making changes to the bones individual properties (pos/rot/scale).
 * To make this work, the `hasChanged` flag in the bone must also be set
 * @param skeleton The skeleton to update
 */
void t3d_skeleton_update(T3DSkeleton *skeleton);

/**
 * Frees data allocated in the skeleton struct.
 * Note: it's safe to call this multiple times, pointers are set to NULL.
 * @param skeleton The skeleton to destroy
 */
void t3d_skeleton_destroy(T3DSkeleton* skeleton);

/**
 * Tries to finds a bone with the given name in the skeleton.
 * Returns the index of the bone or -1 if not found.
 * @param skeleton The skeleton to search in
 * @param name Name of the bone to find
 * @return Index of the bone or -1 if not found
 */
int t3d_skeleton_find_bone(T3DSkeleton* skeleton, const char* name);

/**
 * Draws a skinned model with default settings.
 * Alternatively, use 't3d_model_draw_custom' and set 'matrices' in the config.
 * @param model
 */
static inline void t3d_model_draw_skinned(const T3DModel* model, const T3DSkeleton* skeleton) {
  t3d_model_draw_custom(model, (T3DModelDrawConf){
    .userData = NULL,
    .tileCb = NULL,
    .filterCb = NULL,
    .matrices = skeleton->bufferCount == 1
      ? skeleton->boneMatricesFP
      : (const T3DMat4FP*)t3d_segment_placeholder(T3D_SEGMENT_SKELETON)
  });
}

#ifdef __cplusplus
}
#endif

#endif //TINY3D_T3DSKELETON_H
