#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

/**
 * This shows how you can draw multiple objects (actors) in a scene.
 * Each actor has a scale, rotation, translation (knows as SRT) and a model matrix.
 */
#define ACTOR_COUNT 250

#define RAD_360 6.28318530718f

static float objTimeLast = 0.0f;
static float objTime = 0.0f;
static float baseSpeed = 1.0f;

// Holds our actor data, relevant for t3d is 'modelMat'.
typedef struct {
  uint32_t id;
  float pos[3];
  float rot[3];
  float scale[3];

  rspq_block_t *dpl;
  T3DMat4FP *modelMat;
} Actor;

Actor actor_create(uint32_t id, rspq_block_t *dpl)
{
  float randScale = (rand() % 100) / 3000.0f + 0.03f;
  Actor actor = (Actor){
    .id = id,
    .pos = {0, 0, 0},
    .rot = {0, 0, 0},
    .scale = {randScale, randScale, randScale},
    .dpl = dpl,
    .modelMat = malloc_uncached(sizeof(T3DMat4FP)) // needed for t3d
  };
  t3d_mat4fp_identity(actor.modelMat);
  return actor;
}

void actor_update(Actor *actor) {
  actor->pos[0] = 0;

  // set some random position and rotation
  float randRot = (float)fm_fmodf(actor->id * 123.1f, 5.0f);
  float randDist = (float)fm_fmodf(actor->id * 4645.987f, 30.5f) + 10.0f;

  actor->rot[0] = fm_fmodf(randRot + objTime * 1.05f, RAD_360);
  actor->rot[1] = fm_fmodf(randRot + objTime * 1.03f, RAD_360);
  actor->rot[2] = fm_fmodf(randRot + objTime * 1.1f, RAD_360);

  actor->pos[0] = randDist * fm_cosf(objTime * 1.6f + randDist);
  actor->pos[1] = randDist * fm_sinf(objTime * 1.5f + randRot);
  actor->pos[2] = randDist * fm_cosf(objTime * 1.4f + randDist*randRot);

  // t3d lets you directly construct a fixed-point matrix from SRT
  t3d_mat4fp_from_srt_euler(actor->modelMat, actor->scale, actor->rot, actor->pos);
}

void actor_draw(Actor *actor) {
  t3d_matrix_set(actor->modelMat, true);
  rspq_block_run(actor->dpl);
}

void actor_delete(Actor *actor) {
  free_uncached(actor->modelMat);
}

float get_time_s()  { return (float)((double)get_ticks_ms() / 1000.0); }
float get_time_ms() { return (float)((double)get_ticks_us() / 1000.0); }

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  joypad_init();

  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  // Load a some models
  rspq_block_t *dpls[2];
  T3DModel *models[2] = {
    t3d_model_load("rom:/box.t3dm"),
    t3d_model_load("rom:/food.t3dm")
  };
  const int triangleCount[2] = {12, 60}; // used for the debug overlay

  for(int i=0; i<2; ++i) {
    rspq_block_begin();
    t3d_model_draw(models[i]);
    dpls[i] = rspq_block_end();
  }

  Actor actors[ACTOR_COUNT];
  for(int i=0; i<ACTOR_COUNT; ++i) {
    actors[i] = actor_create(i, dpls[i*3 % 2]);
  }

  const T3DVec3 camPos = {{100.0f,25.0f,0}};
  const T3DVec3 camTarget = {{0,0,0}};

  uint8_t colorAmbient[4] = {80, 50, 50, 0xFF};
  T3DVec3 lightDirVec = {{1.0f, 1.0f, 0.0f}};
  uint8_t lightDirColor[4] = {120, 120, 120, 0xFF};
  t3d_vec3_norm(&lightDirVec);

  int actorCount = 50;

  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);

    if(joypad.btn.c_up)actorCount += 1;
    if(joypad.btn.c_down)actorCount -= 1;
    if(joypad.btn.c_left)actorCount -= 10;
    if(joypad.btn.c_right)actorCount += 10;
    baseSpeed = joypad.stick_y / 80.0f + 1.2f;

    if(actorCount < 0)actorCount = 0;
    if(actorCount > ACTOR_COUNT)actorCount = ACTOR_COUNT;

    float newTime = get_time_s();
    float deltaTime = (newTime - objTimeLast) * baseSpeed;
    objTimeLast = newTime;
    objTime += deltaTime;

    float timeUpdate = get_time_ms();
    for(int i=0; i<actorCount; ++i) {
      actor_update(&actors[i]);
    }
    timeUpdate = get_time_ms() - timeUpdate;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(65.0f), 10.0f, 100.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    rdpq_set_prim_color(RGBA32(0, 0, 0, 0xFF));

    t3d_screen_clear_color(RGBA32(100, 120, 160, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, lightDirColor, &lightDirVec);
    t3d_light_set_count(1);

    t3d_matrix_push_pos(1);
    for(int i=0; i<actorCount; ++i) {
      actor_draw(&actors[i]);
    }
    t3d_matrix_pop(1);

    // ======== Draw (2D) ======== //
    rdpq_sync_pipe();

    int totalTris = 0;
    for(int i=0; i<actorCount; ++i) {
      totalTris += triangleCount[(i*3) % 2];
    }

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 210, "    [C] Actors: %d", actorCount);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 220, "[STICK] Speed : %.2f", baseSpeed);

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 200, 200, "Tris  : %d", totalTris);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 200, 210, "Update: %.2fms", timeUpdate);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 200, 220, "FPS   : %.2f", display_get_fps());

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}

