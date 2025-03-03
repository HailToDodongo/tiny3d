/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "debugMenu.h"
#include <libdragon.h>
#include "../main.h"
#include "../render/debugDraw.h"

namespace
{
  int menuSel = 0;
  constexpr int maxMenuSel = 6;

  constexpr int wrap(int val, int max)
  {
    if(val < 0)return max;
    if(val > max)return 0;
    return val;
  }
}

void DebugMenu::draw(Skybox &skybox)
{
  auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
  if(btn.d_up || btn.c_up)menuSel--;
  if(btn.d_down || btn.c_down)menuSel++;
  if(menuSel < 0)menuSel = maxMenuSel;
  if(menuSel > maxMenuSel)menuSel = 0;

  int selDir = 0;
  if(btn.a || btn.d_right || btn.c_right)selDir = 1;
  if(btn.b || btn.d_left || btn.c_left)selDir = -1;
  if(selDir != 0) {
    switch(menuSel) {
      case 0: state.drawShade = !state.drawShade; break;
      case 1:
        state.currSkybox = wrap(state.currSkybox + selDir, 3);
        skybox.change(state.currSkybox);
      break;
      case 2:
        state.mapModel = wrap(state.mapModel + selDir, 2);
        if(state.mapModel == 2)state.drawShade = false;
      break;
      case 3: state.drawMode = wrap(state.drawMode + selDir, 2); break;
      case 4: state.drawMap = !state.drawMap; break;
      case 5: state.slowCam = !state.slowCam; break;
      case 6: state.limitFPS = !state.limitFPS; break;
    }
  }

  float posX = 20;
  float posY = 18;
  Debug::print(posX, posY, "[START] Menu");
  posY += 16;
  int posYStart = posY;

  Debug::print(posX + 8, posY, state.drawShade ? "Shade: On" : "Shade: -"); posY += 8;
  Debug::printf(posX + 8, posY, "Sky  : %d", state.currSkybox);             posY += 8;
  Debug::printf(posX + 8, posY, "Model: %d", state.mapModel);               posY += 8;
  Debug::printf(posX + 8, posY, "Debug: %d", state.drawMode);               posY += 8;
  Debug::print(posX + 8, posY, state.drawMap ? "Map  : On" : "Map  : -");   posY += 8;
  Debug::print(posX + 8, posY, state.slowCam ? "CamSl: On" : "CamSl: -");   posY += 8;
  Debug::print(posX + 8, posY, state.limitFPS ? "30FPS: On" : "30FPS: -");   posY += 8;

  Debug::print(posX, posYStart + menuSel * 8, ">");
}
