/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include "postProcess.h"

class Scene;

struct State
{
  PostProcessConf ppConf{};
  bool showOffscreen{};
  bool autoExposure{};

  Scene* activeScene{};
};

extern State state;