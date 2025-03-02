/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "skybox.h"

namespace {
  char SKYBOX_PATH[6][21] = {
    "rom:/skybox/0/nz.bci",
    "rom:/skybox/0/nx.bci",
    "rom:/skybox/0/py.bci",
    "rom:/skybox/0/pz.bci",
    "rom:/skybox/0/px.bci",
    "rom:/skybox/0/ny.bci",
  };
}

void Skybox::change(int id) {
  if(id == currId)return;

  currId = id;
  for(int i=0; i<6; ++i) {
    SKYBOX_PATH[i][12] = '0' + id;
    textures.setTexture(i, SKYBOX_PATH[i]);
  }
}