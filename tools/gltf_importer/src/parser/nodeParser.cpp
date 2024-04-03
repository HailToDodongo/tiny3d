/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "parser.h"

Mat4 parseNodeMatrix(const cgltf_node *node, const Vec3 &posScale)
{
  Mat4 matScale{};
  if(node->has_scale)matScale.setScale({node->scale[0], node->scale[1], node->scale[2]});

  Mat4 matRot{};
  if(node->has_rotation)matRot.setRot({
    node->rotation[0],
    node->rotation[1],
    node->rotation[2],
    node->rotation[3]
  });

  Mat4 matTrans{};
  if(node->has_translation) {
    matTrans.setPos({
      node->translation[0] * posScale[0],
      node->translation[1] * posScale[1],
      node->translation[2] * posScale[2],
    });
  };

  Mat4 res = matTrans * matRot * matScale;
  // remove very small values (underflow issues & '-0' values)
  for(int i=0; i<4; ++i) {
    for(int j=0; j<4; ++j) {
      if(fabs(res.data[i][j]) < 0.0001f)res.data[i][j] = 0.0f;
    }
  }

  return res;
}