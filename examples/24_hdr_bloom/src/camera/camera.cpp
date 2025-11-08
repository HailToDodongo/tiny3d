/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "camera.h"

Camera::Camera()
{
  viewport = t3d_viewport_create_buffered(
    display_get_num_buffers()
  );
}

void Camera::update(float deltaTime)
{
  //if(needsProjUpdate) {
    t3d_viewport_set_projection(viewport, fov, near, far);
    //needsProjUpdate = false;
  //}

  t3d_viewport_look_at(viewport, pos, target, T3DVec3{{0, 1, 0}});
}

void Camera::attach() {
  t3d_viewport_attach(viewport);
}
