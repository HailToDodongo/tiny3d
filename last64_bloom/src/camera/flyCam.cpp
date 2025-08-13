/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "flyCam.h"
#include <libdragon.h>

void FlyCam::update(float deltaTime)
{
  auto joypad = joypad_get_inputs(JOYPAD_PORT_1);
  auto joypad_buttons_held = joypad_get_buttons_held(JOYPAD_PORT_1); // Get held buttons

  float smoothFactor = 0.25f;
  if(joypad.btn.c_down) {
    smoothFactor = 0.04f;
  }

  // Deadzone for sticks
  if(joypad.stick_x < 200 && joypad.stick_x > -200)joypad.stick_x = 0;
  if(joypad.stick_y < 200 && joypad.stick_y > -200)joypad.stick_y = 0;
  // Assuming c-stick has similar deadzone handling if needed, but it's not directly accessible via joypad_inputs_t

  float camSpeed = deltaTime * 0.5f;
  float camRotSpeed = deltaTime * 0.015f;

  camRotXCurr = t3d_lerp_angle(camRotXCurr, camRotX, smoothFactor);
  camRotYCurr = t3d_lerp_angle(camRotYCurr, camRotY, smoothFactor);

  T3DVec3 camDir{};
  camDir.v[0] = fm_cosf(camRotXCurr) * fm_cosf(camRotYCurr);
  camDir.v[1] = fm_sinf(camRotYCurr);
  camDir.v[2] = fm_sinf(camRotXCurr) * fm_cosf(camRotYCurr);
  t3d_vec3_norm(&camDir);

  // Movement with left stick (strafing based on current camera orientation)
  // Forward/Backward (Y axis)
  camPos += camDir * ((float)joypad.stick_y * camSpeed);
  // Strafe Left/Right (X axis) - perpendicular to forward direction on the XZ plane
  camPos.v[0] += camDir.v[2] * (float)joypad.stick_x * -camSpeed;
  camPos.v[2] -= camDir.v[0] * (float)joypad.stick_x * -camSpeed;

  // Look-around with C-buttons/C-stick (independent of movement)
  // The C-stick values are not directly available in joypad_inputs_t.
  // We'll assume the C-buttons represent the C-stick deflection for look-around.
  // For a more accurate implementation, libdragon might have specific C-stick access,
  // but using held C-buttons is a common approximation.
  float c_stick_x = 0.0f;
  float c_stick_y = 0.0f;
  const float C_STICK_SENSITIVITY = 500.0f; // Adjust this if C-button "stick" feels too sensitive/fast

  // Map C-button presses to approximate C-stick values for look-around
  // Note: This is a simplification. Real C-stick would provide analog values.
  if (joypad_buttons_held.c_left) c_stick_x = -C_STICK_SENSITIVITY;
  if (joypad_buttons_held.c_right) c_stick_x = C_STICK_SENSITIVITY;
  if (joypad_buttons_held.c_up) c_stick_y = C_STICK_SENSITIVITY;
  if (joypad_buttons_held.c_down) c_stick_y = -C_STICK_SENSITIVITY;

  // Apply look-around rotation with constraints
  camRotX += c_stick_x * camRotSpeed;
  // Constrain vertical look (pitch) to prevent flipping
  // Typical constraint is -89 to 89 degrees. Let's use -80 to 80 for a bit of safety.
  const float PITCH_LIMIT = T3D_DEG_TO_RAD(80.0f);
  camRotY = fminf(fmaxf(camRotY + c_stick_y * camRotSpeed, -PITCH_LIMIT), PITCH_LIMIT);

  // Z-Trigger: No special action in this mode, but could be used for speed boost, etc. if desired.
  // if(joypad.btn.z) { /* Optional Z-trigger action */ }

  t3d_vec3_lerp(cam.pos, cam.pos, camPos, smoothFactor);
  auto actualTarget = cam.pos + camDir;
  cam.target = actualTarget;
}