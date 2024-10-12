#include <libdragon.h>

#include <t3d/t3d.h>
#include <t3d/t3dmath.h>

/**
 * Simple example with a spinning quad.
 * This shows how to manually generate geometry and draw it,
 * although most o the time you should use the builtin model format.
 */
int main()
{
	debug_init_isviewer();
	debug_init_usblog();

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
  rdpq_init();

  t3d_init((T3DInitParams){}); // Init library itself, use empty params for default settings

  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  t3d_mat4_identity(&modelMat);
  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  const T3DVec3 camPos = {{0,0,-18}};
  const T3DVec3 camTarget = {{0,0,0}};

  uint8_t colorAmbient[4] = {50, 50, 50, 0xFF};
  uint8_t colorDir[4]     = {0xFF, 0xFF, 0xFF, 0xFF};

  T3DVec3 lightDirVec = {{0.0f, 0.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  // Allocate vertices (make sure to have an uncached pointer before passing it to the API!)
  // For performance reasons, 'T3DVertPacked' contains two vertices at once in one struct.
  T3DVertPacked* vertices = malloc_uncached(sizeof(T3DVertPacked) * 2);

  uint16_t norm = t3d_vert_pack_normal(&(T3DVec3){{ 0, 0, 1}}); // normals are packed in a 5.6.5 format
  vertices[0] = (T3DVertPacked){
    .posA = {-16, -16, 0}, .rgbaA = 0xFF0000'FF, .normA = norm,
    .posB = { 16, -16, 0}, .rgbaB = 0x00FF00'FF, .normB = norm,
  };
  vertices[1] = (T3DVertPacked){
    .posA = { 16,  16, 0}, .rgbaA = 0x0000FF'FF, .normA = norm,
    .posB = {-16,  16, 0}, .rgbaB = 0xFF00FF'FF, .normB = norm,
  };

  float rotAngle = 0.0f;
  T3DVec3 rotAxis = {{-1.0f, 2.5f, 0.25f}};
  t3d_vec3_norm(&rotAxis);

  // create a viewport, this defines the section to draw to (by default the whole screen)
  // and contains the projection & view (camera) matrices
  T3DViewport viewport = t3d_viewport_create();

  rspq_block_t *dplDraw = NULL;

  for(;;)
  {
    // ======== Update ======== //
    rotAngle += 0.03f;

    // we can set up our viewport settings beforehand here
    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 100.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // Model-Matrix, t3d offers some basic matrix functions
    t3d_mat4_identity(&modelMat);
    t3d_mat4_rotate(&modelMat, &rotAxis, rotAngle);
    t3d_mat4_scale(&modelMat, 0.4f, 0.4f, 0.4f);
    t3d_mat4_to_fixed(modelMatFP, &modelMat);

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf()); // set the target to draw to
    t3d_frame_start(); // call this once per frame at the beginning of your draw function

    t3d_viewport_attach(&viewport); // now use the viewport, this applies proj/view matrices and sets scissoring

    rdpq_mode_combiner(RDPQ_COMBINER_SHADE);
    // this cleans the entire screen (even if out viewport is smaller)
    t3d_screen_clear_color(RGBA32(100, 0, 100, 0));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient); // one global ambient light, always active
    t3d_light_set_directional(0, colorDir, &lightDirVec); // optional directional light, can be disabled
    t3d_light_set_count(1);

    t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_DEPTH);

    // t3d functions can be recorded into a display list:
    if(!dplDraw) {
      rspq_block_begin();

      t3d_matrix_push(modelMatFP); // Matrix load can be recorded as they DMA the data in internally
      t3d_vert_load(vertices, 0, 4); // load 4 vertices...
      t3d_matrix_pop(1); // ...and pop the matrix, this can be done as soon as the vertices are loaded...
      t3d_tri_draw(0, 1, 2); // ...then draw 2 triangles
      t3d_tri_draw(2, 3, 0);

      // NOTE: if you use the builtin model format, syncs are handled automatically!
      t3d_tri_sync(); // after each batch of triangles, a sync is needed
      // technically, you only need a sync before any new 't3d_vert_load', rdpq call, or after the last triangle
      // for safety, just call it after you are done with all triangles after a load

      dplDraw = rspq_block_end();
    }

    rspq_block_run(dplDraw);

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

