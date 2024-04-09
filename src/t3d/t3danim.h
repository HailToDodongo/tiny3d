/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#ifndef TINY3D_T3DANIM_H
#define TINY3D_T3DANIM_H

#include "t3dmodel.h"
#include "t3dskeleton.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define T3D_ANIM_TARGET_TRANSLATION 0
#define T3D_ANIM_TARGET_SCALE_XYZ   1
#define T3D_ANIM_TARGET_SCALE_S     2
#define T3D_ANIM_TARGET_ROTATION    3

typedef struct {
  void* target; // points to either a 'float' or 'Quat'
  int* changedFlag; // flag to increment when target is changed
} T3DAnimTarget;

typedef struct {
  T3DChunkAnim *animRef;
  T3DAnimTarget *targets;

  float speed;
  float time;

  float timeNextPage;
  int loadedPageIdx;
  char* pageData;
} T3DAnim;

/**
 * Creates an animation instance from a model's animation definition
 * @param model The model to create the animation from
 * @param name The name of the animation to create
 * @return The created animation
 */
T3DAnim t3d_anim_create(const T3DModel *model, const char* name);

/**
 * Attaches an animation to a skeleton.
 * @param anim The animation to attach
 * @param skeleton The skeleton to attach the animation to
 */
void t3d_anim_attach(T3DAnim* anim, const T3DSkeleton* skeleton);

/**
 * Updates an animation, this will actually play it and apply the changes to the skeleton.
 * @param anim animation to update
 * @param deltaTime time since last update (seconds)
 */
void t3d_anim_update(T3DAnim* anim, float deltaTime);

/**
 * Frees data allocated in the animation struct.
 * @param anim
 */
void t3d_anim_destroy(T3DAnim *anim);

#ifdef __cplusplus
}
#endif

#endif // TINY3D_T3DANIM_H