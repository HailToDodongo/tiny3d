/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "debugMenu.h"
#include <libdragon.h>
#include "render/debugDraw.h"
#include "scene/sceneManager.h"
#include <vector>

namespace
{
  constinit int menuSel{};
  constinit int maxMenuSel{};
  constinit int idxCustom{};

  constinit int sceneId{};
  constinit bool needsSceneLoad{false};

  template<typename T>
  constexpr T clamp(T val, T min, T max)
  {
    if(val < min) return min;
    if(val > max) return max;
    return val;
  }

  std::vector<DebugMenu::Entry> entries{};
  std::vector<bool*> changedFlags{};
}

void DebugMenu::reset()
{
  entries.clear();
  changedFlags.clear();

  entries.push_back({"Scene", EntryType::INT, &sceneId, 0, 5});
  entries.push_back({"Debug", EntryType::BOOL, &state.showOffscreen});
  entries.push_back({"Blurs", EntryType::INT, &state.ppConf.blurSteps, 0, 50});
  entries.push_back({"Bloom", EntryType::FLOAT, &state.ppConf.blurBrightness, 0.0f, 8.0f, 0.01f});
  entries.push_back({"Expos", EntryType::FLOAT, &state.ppConf.hdrFactor, 0.0f, 8.0f, 0.03f});
  entries.push_back({"Thres", EntryType::FLOAT, &state.ppConf.bloomThreshold, 0.0f, 1.0f, 1.0f/256.0f});
  entries.push_back({"RDP-S", EntryType::BOOL, &state.ppConf.scalingUseRDP});
  entries.push_back({"Auto ", EntryType::BOOL, &state.autoExposure});

  changedFlags.resize(entries.size());
  changedFlags[0] = &needsSceneLoad;

  menuSel = 0;
  idxCustom = entries.size();
}

void DebugMenu::addEntry(const Entry& entry, bool *changedFlag) {
  entries.push_back(entry);
  changedFlags.resize(entries.size());
  changedFlags[entries.size()-1] = changedFlag;
  // menuSel = entries.size()-1;
}

void DebugMenu::draw()
{
  auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
  auto held = joypad_get_buttons_held(JOYPAD_PORT_1);

  if(btn.l && sceneId > entries[0].min) {
    sceneId--; needsSceneLoad = true;
  }
  if(btn.r && sceneId < entries[0].max) {
    sceneId++; needsSceneLoad = true;
  }

  if(needsSceneLoad) {
    SceneManager::loadScene(sceneId);
    needsSceneLoad = false;
  }

  maxMenuSel = entries.size()-1;
  if(btn.d_up || btn.c_up)menuSel--;
  if(btn.d_down || btn.c_down)menuSel++;
  if(menuSel < 0)menuSel = maxMenuSel;
  if(menuSel > maxMenuSel)menuSel = 0;

  int selDir = 0;
  if(btn.a || btn.d_right || btn.c_right)selDir = 1;
  if(btn.b || btn.d_left || btn.c_left)selDir = -1;

  int heldDir = 0;
  if(held.d_right || held.c_right)heldDir = 1;
  if(held.d_left || held.c_left)heldDir = -1;

  Entry &curr = entries[menuSel];
  switch(curr.type) {
    case EntryType::INT:
      if(selDir != 0) {
        int *value = (int*)curr.value;
        *value = clamp(*value + selDir, (int)curr.min, (int)curr.max);
        if(changedFlags[menuSel])*changedFlags[menuSel] = true;
      }
      break;
    case EntryType::FLOAT:
      if(heldDir != 0) {
        float *value = (float*)curr.value;
        *value = clamp(*value + heldDir * curr.incr * (held.z ? 4.0f : 1.0f), curr.min, curr.max);
        if(changedFlags[menuSel])*changedFlags[menuSel] = true;
      }
      break;
    case EntryType::BOOL:
      if(selDir != 0) {
        bool *value = (bool*)curr.value;
        *value = !(*value);
        if(changedFlags[menuSel])*changedFlags[menuSel] = true;
      }
      break;
  }

  float posX = 20;
  float posY = 18;
  Debug::print(posX, posY, "[START] Menu");
  Debug::print(display_get_width() - 100, posY, "[L/R] Scene");
  posY += 12;

  int idx = 0;
  for(auto &entry : entries) {
    if(idx == idxCustom) {
      Debug::print(posX, posY+8, "Scene:");
      posY += 12+8;
    }

    switch(entry.type) {
      case EntryType::INT:
        if (entry.value == &sceneId) {
          // Special handling for scene selection - Updated order to match SceneManager
          const char* sceneNames[] = {"Last64", "Main", "Env", "Magic", "Pixel", "Multiplayer"};
          int sceneIdx = *(int*)entry.value;
          if (sceneIdx >= 0 && sceneIdx <= 5) {
            Debug::printf(posX + 8, posY, "%s: %d (%s)", entry.name, sceneIdx, sceneNames[sceneIdx]);
          } else {
            Debug::printf(posX + 8, posY, "%s: %d", entry.name, sceneIdx);
          }
        } else {
          Debug::printf(posX + 8, posY, "%s: %d", entry.name, *(int*)entry.value);
        }
        break;
      case EntryType::FLOAT:
        Debug::printf(posX + 8, posY, "%s: %.2f", entry.name, *(float*)entry.value);
        break;
      case EntryType::BOOL:
        Debug::printf(posX + 8, posY, *((bool*)entry.value) ? "%s: ON" : "%s: OFF", entry.name);
        break;
    }

    if(menuSel == idx) {
      Debug::print(posX, posY, ">");
    }

    posY += 8;
    ++idx;
  }
}
