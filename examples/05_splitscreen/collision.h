/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

typedef struct {
  float min[2];
  float max[2];
} AABB;

#define MAP_COLL_AABB_COUNT 3
#define AABB_BORDER 5

// 2D AABBs (top-down view) for the map
// to avoid any custom map export shenanigans, we just hardcode the AABBs here
static AABB mapColl[MAP_COLL_AABB_COUNT] = {
  {{ -25-AABB_BORDER, -78-AABB_BORDER}, { 25+AABB_BORDER, 78+AABB_BORDER}},
  {{-102-AABB_BORDER, -27-AABB_BORDER}, {-50+AABB_BORDER, 27+AABB_BORDER}},
  {{  50-AABB_BORDER, -27-AABB_BORDER}, {102+AABB_BORDER, 27+AABB_BORDER}}
};

static void resolve_aabb_collision(AABB *aabb, T3DVec3 *pos) {
  // Check if the point is inside the AABB
  if(pos->v[0] >= aabb->min[0] && pos->v[0] <= aabb->max[0]
  && pos->v[2] >= aabb->min[1] && pos->v[2] <= aabb->max[1])
  {
    // find the closest edge and move the point to it
    // keep in mind that that is a rather simplistic approach to keep the example simple
    float dxMin = fabsf(pos->v[0] - aabb->min[0]);
    float dxMax = fabsf(pos->v[0] - aabb->max[0]);
    float dzMin = fabsf(pos->v[2] - aabb->min[1]);
    float dzMax = fabsf(pos->v[2] - aabb->max[1]);

    float minDist = fminf(fminf(dxMin, dxMax), fminf(dzMin, dzMax));

    if(minDist == dxMin){ pos->v[0] = aabb->min[0]; return; }
    if(minDist == dxMax){ pos->v[0] = aabb->max[0]; return; }
    if(minDist == dzMin){ pos->v[2] = aabb->min[1]; return; }
    if(minDist == dzMax){ pos->v[2] = aabb->max[1]; return; }
  }
}

static void check_map_collision(T3DVec3 *pos) {
  // in a real-world scenario, you would use a more sophisticated collision logic
  // for example, by having a broad-phase to pre-filter AABBs
  for(int i=0; i<MAP_COLL_AABB_COUNT; ++i) {
    // to simplify even further, we only check each AABB in order once
    // usually one AABB resolver could move the point into another AABB, that is ignored here
    resolve_aabb_collision(&mapColl[i], pos);
  }
}