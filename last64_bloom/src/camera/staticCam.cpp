/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "staticCam.h"
#include <t3d/t3dmath.h>

StaticCam::StaticCam(Camera &camera) : cam(camera)
{
  position = {0, 0, 0};
  target = {0, 0, 0};
  height = 50.0f;
  distance = 30.0f;
  angle = 0.0f;
}

void StaticCam::update(float deltaTime)
{
  // Calculate camera position based on target, height, distance, and angle
  float camX = target.x + distance * fm_sinf(angle);
  float camZ = target.z + distance * fm_cosf(angle);
  
  position.x = camX;
  position.y = target.y + height;
  position.z = camZ;
  
  // Update the camera's position and target
  cam.pos = position;
  cam.target = target;
}

void StaticCam::setTarget(T3DVec3 newTarget)
{
  target = newTarget;
}

void StaticCam::setPosition(T3DVec3 newPosition)
{
  position = newPosition;
  // Update camera directly
  cam.pos = position;
}

void StaticCam::setHeight(float newHeight)
{
  height = newHeight;
}

void StaticCam::setDistance(float newDistance)
{
  distance = newDistance;
}

void StaticCam::setAngle(float newAngle)
{
  angle = newAngle;
}