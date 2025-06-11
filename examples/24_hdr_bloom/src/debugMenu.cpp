/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "debugMenu.h"
#include <libdragon.h>
#include "render/debugDraw.h"

namespace
{
  int menuSel = 0;
  constexpr int maxMenuSel = 4;

  template<typename T>
  constexpr T clamp(T val, T min, T max)
  {
    if(val < min) return min;
    if(val > max) return max;
    return val;
  }
}

void DebugMenu::draw(State &state)
{
  auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
  auto held = joypad_get_buttons_held(JOYPAD_PORT_1);

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

  if(selDir != 0) {
    switch(menuSel) {
      case 0: state.showOffscreen = !state.showOffscreen; break;
      case 1: state.ppConf.blurSteps = clamp(state.ppConf.blurSteps + selDir, 0, 50); break;
    }
  }
  if(heldDir != 0) {
    switch(menuSel) {
      case 2: state.ppConf.blurBrightness = clamp(state.ppConf.blurBrightness + heldDir*0.01f, 0.0f, 8.0f); break;
      case 3: state.ppConf.hdrFactor = clamp(state.ppConf.hdrFactor + heldDir*0.04f, 0.0f, 8.0f); break;
      case 4: state.ppConf.bloomThreshold = clamp(state.ppConf.bloomThreshold + heldDir*(1.0f/256.0f), 0.0f, 1.0f); break;
    }
  }

  float posX = 20;
  float posY = 18;
  Debug::print(posX, posY, "[START] Menu");
  posY += 12;
  int posYStart = posY;

  Debug::print(posX + 8, posY, state.showOffscreen ? "Debug: On" : "Debug: -"); posY += 8;
  Debug::printf(posX + 8, posY, "Blurs: %d", state.ppConf.blurSteps);         posY += 8;
  Debug::printf(posX + 8, posY, "Bloom: %.2f", state.ppConf.blurBrightness); posY += 8;
  Debug::printf(posX + 8, posY, "Expos: %.2f", state.ppConf.hdrFactor);     posY += 8;
  Debug::printf(posX + 8, posY, "Thres: %.2f", state.ppConf.bloomThreshold); posY += 8;

  Debug::print(posX, posYStart + menuSel * 8, ">");
}
