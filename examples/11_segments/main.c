#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>

/**
 * Example project showcasing segmented addresses for skeletons and model matrices.
 * This can be used to perform buffering of matrices across frames as an alternative to sync-points.
 *
 * Practically speaking, this means allocating a matrix per framebuffer for an object
 * which is then cycled through each frame.
 * This prevents the CPU from modifying the matrix while the last frame is still being processed.
 *
 * The segment system itself works by encoding a segment-id into a pointer.
 * That pointer can then be used for e.g. matrix loads, even inside recorded blocks.
 * In addition, there is a segment table kept on the RSP, which can be updated at any time.
 * On a matrix load, the provided address is added together with the address in the segment table.
 * This allows for both placeholders (aka NULL pointer that get set later), and relative addressing.
 */

float get_time_s() {
  return (float)((double)get_ticks_us() / 1000000.0);
}

int main()
{
  debug_init_isviewer();
  debug_init_usblog();
  asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  t3d_init((T3DInitParams){});

  T3DViewport viewport = t3d_viewport_create();

  // allocate a matrix per frame here, this can be done in a single buffer
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP) * display_get_num_buffers());

  T3DVec3 camPos    = {{0, 100, 140}};
  T3DVec3 camTarget = {{0, 100, 0}};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  uint8_t colorAmbient[4] = {0xFF, 0xFF, 0xFF, 0xFF};

  // Model Credits: Quaternius (CC0) https://quaternius.com/packs/ultimatemonsters.html
  T3DModel *modelPlayer = t3d_model_load("rom:/player.t3dm");

  // for skinned meshes (which contains a matrix per bone) we can use this helper function to do a buffered allocation
  T3DSkeleton skel = t3d_skeleton_create_buffered(modelPlayer, display_get_num_buffers());

  T3DAnim animFly = t3d_anim_create(modelPlayer, "Fast_Flying");
  t3d_anim_attach(&animFly, &skel);

  rspq_block_begin();
    // instead of directly setting a matrix, we can use a placeholder segment.
    // this returns a segmented dummy pointer that can be set later before the actual draw
    t3d_matrix_push(t3d_segment_placeholder(1));
    t3d_model_draw_skinned(modelPlayer, &skel); // for the skeleton, no special function is required
    t3d_matrix_pop(1);
  rspq_block_t *dplPlayer = rspq_block_end();

  float lastTime = get_time_s() - (1.0f / 60.0f);

  for(uint32_t frame=0; ;++frame)
  {
    // ======== Update ======== //
    float newTime = get_time_s();
    float deltaTime = newTime - lastTime;
    lastTime = newTime;

    t3d_anim_update(&animFly, deltaTime);
    // the skeleton update also stays the same, internally it will handle the switch between buffers
    t3d_skeleton_update(&skel);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(80.0f), 5.0f, 180.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // for our model matrix, determine an index based on the frame...
    uint32_t matrixIdx = frame % display_get_num_buffers();

    float posX = sinf(newTime) * 65.0f;
    float rotY = fm_fmodf(newTime * 0.75f, T3D_PI * 2.0f);
    t3d_mat4fp_from_srt_euler(&modelMatFP[matrixIdx], //...then update that specific matrix
      (float[3]){0.8f, 0.8f, 0.8f},
      (float[3]){0, rotY, 0},
      (float[3]){posX, 0, 0}
    );

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();

    t3d_viewport_attach(&viewport);
    t3d_screen_clear_color(RGBA32(140, 227, 237, 0xFF));
    t3d_screen_clear_depth();
    t3d_light_set_count(0);

    rdpq_set_prim_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
    t3d_light_set_ambient(colorAmbient);

    // To fill the placeholder for our model matrix, set the segment to the correct matrix
    t3d_segment_set(1, &modelMatFP[matrixIdx]);

    // For skeletons, there is a helper function to do so:
    // (Note that for un-buffered skeletons this is a no-op, but still safe to call)
    t3d_skeleton_use(&skel);
    rspq_block_run(dplPlayer);

    rdpq_detach_show();
  }
}

