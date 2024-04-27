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
  float timeStart;
  float timeEnd;
  int32_t* changedFlag; // flag to increment when target is changed
} T3DAnimTargetBase;

typedef struct {
  T3DAnimTargetBase base;
  T3DQuat* targetQuat; // target to modify
  T3DQuat kfCurr; // current keyframe value
  T3DQuat kfNext; // next keyframe value
} T3DAnimTargetQuat;

typedef struct {
  T3DAnimTargetBase base;
  float* targetScalar;
  float kfCurr;
  float kfNext;
} T3DAnimTargetScalar;

typedef struct {
  T3DChunkAnim *animRef;
  T3DAnimTargetQuat *targetsQuat;
  T3DAnimTargetScalar *targetsScalar;

  float speed;
  float time;

  FILE *file;
  int nextKfSize;
  uint8_t isPlaying;
  uint8_t isLooping;
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
 * Attaches a single position target to a single channel target.
 * This can be used to override an earlier attachment from 't3d_anim_attach'.
 *
 * @param anim animation to attach to
 * @param targetIdx index of the target bone
 * @param target position to update
 * @param updateFlag set to '1' if changed, '2' if animation rolled over
 */
void t3d_anim_attach_pos(T3DAnim* anim, uint32_t targetIdx, T3DVec3* target, int32_t *updateFlag);

/**
 * Attaches a single rotation target to a single channel target.
 * This can be used to override an earlier attachment from 't3d_anim_attach'.
 *
 * @param anim animation to attach to
 * @param targetIdx index of the target bone
 * @param target rotation to update
 * @param updateFlag set to '1' if changed, '2' if animation rolled over
 */
void t3d_anim_attach_rot(T3DAnim* anim, uint32_t targetIdx, T3DQuat* target, int32_t *updateFlag);

/**
 * Attaches a single scale target to a single channel target.
 * This can be used to override an earlier attachment from 't3d_anim_attach'.
 *
 * @param anim animation to attach to
 * @param targetIdx index of the target bone
 * @param target scale to update
 * @param updateFlag set to '1' if changed, '2' if animation rolled over
 */
void t3d_anim_attach_scale(T3DAnim* anim, uint32_t targetIdx, T3DVec3* target, int32_t *updateFlag);

/**
 * Updates an animation, this will actually play it and apply the changes to the skeleton.
 * @param anim animation to update
 * @param deltaTime time since last update (seconds)
 */
void t3d_anim_update(T3DAnim* anim, float deltaTime);

/**
 * Sets the animation to a specific time.
 * Note: this may cause some work internally due to potential DMAs.
 * @param anim animation to set time for
 * @param time time in seconds
 */
void t3d_anim_set_time(T3DAnim* anim, float time);

/**
 * Sets the speed of the animation.
 * Note: reverse playback (speed < 0) is currently not supported.
 * @param anim animation to set speed for
 * @param speed speed as a factor, default: 1.0
 */
inline static void t3d_anim_set_speed(T3DAnim* anim, float speed) {
  anim->speed = speed < 0.0f ? 0.0f : speed;
}

/**
 * Set the animation to playing or paused.
 * Note: this works independently of setting the speed to 0.
 * @param anim
 * @param isPlaying true to play, false to pause
 */
inline static void t3d_anim_set_playing(T3DAnim* anim, bool isPlaying) {
  anim->isPlaying = isPlaying;
}

/**
 * Sets the animation to loop or not.
 * If the animation is not set to loop, you will have to call 't3d_anim_set_playing' to play it again.
 * @param anim animation to set loop for
 * @param loop true to loop, false to stop at the end
 */
inline static void t3d_anim_set_looping(T3DAnim* anim, bool loop) {
  anim->isLooping = loop;
}

/**
 * Frees data allocated in the animation struct.
 * @param anim
 */
void t3d_anim_destroy(T3DAnim *anim);

#ifdef __cplusplus
}
#endif

#endif // TINY3D_T3DANIM_H