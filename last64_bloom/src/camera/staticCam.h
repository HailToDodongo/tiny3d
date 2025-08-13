/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "camera.h"

struct StaticCam
{
  Camera &cam;
  
  // Position of the camera (hovering above the scene)
  T3DVec3 position;
  
  // Target position to look at (center of the scene)
  T3DVec3 target;
  
  // Height above the target position
  float height;
  
  // Distance from the target position
  float distance;
  
  // Rotation around the target (horizontal)
  float angle;

  StaticCam(Camera &camera);
  
  void update(float deltaTime);
  
  void setTarget(T3DVec3 newTarget);
  void setPosition(T3DVec3 newPosition);
  void setHeight(float newHeight);
  void setDistance(float newDistance);
  void setAngle(float newAngle);
};