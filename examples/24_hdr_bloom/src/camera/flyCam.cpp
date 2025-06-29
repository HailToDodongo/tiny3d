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

  camRotXCurr = t3d_lerp_angle(camRotXCurr, camRotX, 0.1f);
  camRotYCurr = t3d_lerp_angle(camRotYCurr, camRotY, 0.1f);

  T3DVec3 camDir{};
  camDir.v[0] = fm_cosf(camRotXCurr) * fm_cosf(camRotYCurr);
  camDir.v[1] = fm_sinf(camRotYCurr);
  camDir.v[2] = fm_sinf(camRotXCurr) * fm_cosf(camRotYCurr);
  t3d_vec3_norm(&camDir);

  if(joypad.btn.z) {
    camRotX += (float)joypad.stick_x * camRotSpeed;
    camRotY += (float)joypad.stick_y * camRotSpeed;
  } else {
    camPos += camDir * ((float)joypad.stick_y * camSpeed);
    camPos.v[0] += camDir.v[2] * (float)joypad.stick_x * -camSpeed;
    camPos.v[2] -= camDir.v[0] * (float)joypad.stick_x * -camSpeed;
  }

  t3d_vec3_lerp(cam.pos, cam.pos, camPos, 0.1f);
  auto actualTarget = cam.pos + camDir;
  cam.target = actualTarget;
}