/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneManager.h"
#include "../main.h"

#include "../memory/matrixManager.h"
#include "../debugMenu.h"

#include "scenes/sceneMain.h"
#include "scenes/sceneMagic.h"
#include "scenes/sceneEnv.h"
#include "scenes/scenePixel.h"
#include "scenes/sceneLast64.h"
#include "scenes/sceneMP.h"

namespace {
  constinit int requestSceneId{-1};
  constinit heap_stats_t heapStats{};

  int32_t getHeapDiff()
  {
    auto oldStats = heapStats;
    sys_get_heap_stats(&heapStats);
    if(oldStats.total == 0)return 0; // first call
    return heapStats.used - oldStats.used;
  }
}

void SceneManager::loadScene(int sceneId)
{
  requestSceneId = sceneId;
}

void SceneManager::update()
{
  if(requestSceneId >= 0) {
    rspq_wait();

    MatrixManager::reset();
    DebugMenu::reset();

    if(state.activeScene) {
      delete state.activeScene;
    }
    debugf("Loading scene %d (heap-diff: %ld)\n", requestSceneId, getHeapDiff());

    switch(requestSceneId) {
      case 0: state.activeScene = new SceneLast64(); break; // Make SceneLast64 index 0
      case 1: state.activeScene = new SceneMain(); break;
      case 2: state.activeScene = new SceneEnv(); break;
      case 3: state.activeScene = new SceneMagic(); break;
      case 4: state.activeScene = new ScenePixel(); break;
      case 5: state.activeScene = new SceneMP(); break; // Our new multiplayer scene

      default: assertf(false, "Invalid scene-id: %d", requestSceneId);
    }

    requestSceneId = -1;
  }
}