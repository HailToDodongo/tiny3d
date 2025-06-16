/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "flyCam.h"


void FlyCam::update(float deltaTime)
{
  auto joypad = joypad_get_inputs(JOYPAD_PORT_1);
  if(joypad.stick_x < 10 && joypad.stick_x > -10)joypad.stick_x = 0;
  if(joypad.stick_y < 10 && joypad.stick_y > -10)joypad.stick_y = 0;

  float camSpeed = deltaTime * 0.5f;
  float camRotSpeed = deltaTime * 0.015f;

  T3DVec3 camDir{};
  camDir.v[0] = fm_cosf(camRotX) * fm_cosf(camRotY);
  camDir.v[1] = fm_sinf(camRotY);
  camDir.v[2] = fm_sinf(camRotX) * fm_cosf(camRotY);
  t3d_vec3_norm(&camDir);

  if(joypad.btn.z) {
    camRotX += (float)joypad.stick_x * camRotSpeed;
    camRotY += (float)joypad.stick_y * camRotSpeed;
  } else {
    cam.pos += camDir * ((float)joypad.stick_y * camSpeed);

    cam.pos.v[0] += camDir.v[2] * (float)joypad.stick_x * -camSpeed;
    cam.pos.v[2] -= camDir.v[0] * (float)joypad.stick_x * -camSpeed;
  }

  cam.target = cam.pos + camDir;
  cam.update(deltaTime);
}