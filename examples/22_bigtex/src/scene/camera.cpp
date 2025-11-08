/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "camera.h"
#include "../main.h"

Camera::Camera()
  : pos{{2.f, 20.0f, 110.0f}}, targetPos{pos},
    rotX{1.544792654048f},
    rotY{3.05f}
{
  viewport = t3d_viewport_create_buffered(3);
  viewport.size[0] = SCREEN_WIDTH;
  viewport.size[1] = SCREEN_HEIGHT;
}

void Camera::update(float deltaTime) {
  auto joypad = joypad_get_inputs(JOYPAD_PORT_1);

  float camSpeed = deltaTime * 1.0f;
  float camRotSpeed = deltaTime * 0.03f;

  dir.v[0] = fm_cosf(rotX) * fm_cosf(rotY);
  dir.v[1] = fm_sinf(rotY);
  dir.v[2] = fm_sinf(rotX) * fm_cosf(rotY);
  t3d_vec3_norm(&dir);

  if(joypad.btn.z) {
    targetRotX += (float)joypad.stick_x * camRotSpeed;
    targetRotY += (float)joypad.stick_y * camRotSpeed;
  } else {
    targetPos.v[0] += dir.v[0] * (float)joypad.stick_y * camSpeed;
    targetPos.v[1] += dir.v[1] * (float)joypad.stick_y * camSpeed;
    targetPos.v[2] += dir.v[2] * (float)joypad.stick_y * camSpeed;

    targetPos.v[0] += dir.v[2] * (float)joypad.stick_x * -camSpeed;
    targetPos.v[2] -= dir.v[0] * (float)joypad.stick_x * -camSpeed;
  }

  if(joypad.btn.c_right) {
    targetPos.v[0] -= dir.v[2] * camSpeed * 32.0f;
    targetPos.v[2] += dir.v[0] * camSpeed * 32.0f;
  }
  if(joypad.btn.c_left) {
    targetPos.v[0] += dir.v[2] * camSpeed * 32.0f;
    targetPos.v[2] -= dir.v[0] * camSpeed * 32.0f;
  }

  if(joypad.btn.c_up)targetPos.v[1] += camSpeed * 32.0f;
  if(joypad.btn.c_down)targetPos.v[1] -= camSpeed * 32.0f;

  if(camLerpSpeed > 0.0f) {
    float lerpFactor = 1.0f - powf(camLerpSpeed, deltaTime);
    t3d_vec3_lerp(pos, pos, targetPos, lerpFactor);
    rotX = t3d_lerp(rotX, targetRotX, lerpFactor);
    rotY = t3d_lerp(rotY, targetRotY, lerpFactor);
  } else {
    pos = targetPos;
    rotX = targetRotX;
    rotY = targetRotY;
  }

  target.v[0] = pos.v[0] + dir.v[0];
  target.v[1] = pos.v[1] + dir.v[1];
  target.v[2] = pos.v[2] + dir.v[2];

  t3d_viewport_set_projection(viewport, T3D_DEG_TO_RAD(70.0f), 4.0f, 180.0f);
  t3d_viewport_look_at(viewport, pos, target, {0,1,0});
}

void Camera::attach() {
  t3d_viewport_attach(viewport);
}
