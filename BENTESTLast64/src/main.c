#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

/**
 * Last64 - A minimalist Vampire Survivors-like game
 * Using tiny3d for abstract geometric visuals
 */
int main()
{
    // Initialize debugging if needed
    debug_init_isviewer();
    debug_init_usblog();
    asset_init_compression(2);

    // Initialize filesystem to load models
    dfs_init(DFS_DEFAULT_LOCATION);

    // Initialize display with bloom-friendly settings
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
    rdpq_init();

    // Initialize tiny3d and create viewport
    t3d_init((T3DInitParams){});
    T3DViewport viewport = t3d_viewport_create();

    // Create and setup matrices
    T3DMat4 modelMat;
    t3d_mat4_identity(&modelMat);
    T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

    // Setup camera
    const T3DVec3 camPos = {{0,10.0f,40.0f}};
    const T3DVec3 camTarget = {{0,0,0}};

    // Setup lighting
    uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};
    uint8_t colorDir[4] = {0xEE, 0xAA, 0xAA, 0xFF};
    T3DVec3 lightDirVec = {{-1.0f, 1.0f, 1.0f}};
    t3d_vec3_norm(&lightDirVec);

    // Load the box model
    T3DModel *model = t3d_model_load("rom:/lowpoly_skinned_mario.t3dm");
    float rotAngle = 0.0f;
    rspq_block_t *dplDraw = NULL;

    // Main game loop
    while (1)
    {
        // Update rotation and scale
        rotAngle -= 0.02f;
        float modelScale = 0.1f;

        // Update viewport and camera
        t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
        t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

        // Update model matrix
        t3d_mat4_from_srt_euler(&modelMat,
            (float[3]){modelScale, modelScale, modelScale},
            (float[3]){0.0f, rotAngle*0.2f, rotAngle},
            (float[3]){0,0,0}
        );
        t3d_mat4_to_fixed(modelMatFP, &modelMat);

        // Begin rendering
        rdpq_attach(display_get(), display_get_zbuf());
        t3d_frame_start();
        t3d_viewport_attach(&viewport);

        // Clear screen
        t3d_screen_clear_color(RGBA32(100, 80, 80, 0xFF));
        t3d_screen_clear_depth();

        // Set lighting
        t3d_light_set_ambient(colorAmbient);
        t3d_light_set_directional(0, colorDir, &lightDirVec);
        t3d_light_set_count(1);

        // Draw model
        if(!dplDraw) {
            rspq_block_begin();
            t3d_matrix_push(modelMatFP);
            t3d_model_draw(model);
            t3d_matrix_pop(1);
            dplDraw = rspq_block_end();
        }
        rspq_block_run(dplDraw);

        // Show frame
        rdpq_detach_show();
    }

    t3d_destroy();
    return 0;
}
