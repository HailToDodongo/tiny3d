/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "optimizer.h"

#include "bvh/v2/bvh.h"
#include "bvh/v2/vec.h"
#include "bvh/v2/ray.h"
#include "bvh/v2/node.h"
#include "bvh/v2/default_builder.h"

using Scalar  = double;
using BVec3    = bvh::v2::Vec<Scalar, 3>;
using BBox    = bvh::v2::BBox<Scalar, 3>;
using Node    = bvh::v2::Node<Scalar, 3>;
using Bvh     = bvh::v2::Bvh<Node>;

namespace
{
  void writeBVHNode(std::vector<int16_t> &out, Node &node, int nodeIndex) {
    // 'bounds' layout is [min_x, max_x, min_y, max_y, min_z, max_z]
    // we need min/max as separate vectors
    out.push_back((int16_t)node.bounds[0]);
    out.push_back((int16_t)node.bounds[2]);
    out.push_back((int16_t)node.bounds[4]);
    out.push_back((int16_t)node.bounds[1]);
    out.push_back((int16_t)node.bounds[3]);
    out.push_back((int16_t)node.bounds[5]);

    int dataCount = node.index.value & 0b1111;
    int dataOffset = node.index.value >> 4;

    if(dataCount == 0) {
      int indexDiff = dataOffset - nodeIndex;
      int16_t packedVal = (int16_t)(indexDiff << 4);
      assert((packedVal >> 4) == indexDiff);
      out.push_back(packedVal);
    } else {
      out.push_back(node.index.value);
    }
  }

  void writeBVH(std::vector<int16_t> &out, Bvh &bvh) {
    out.push_back(bvh.nodes.size());
    out.push_back(bvh.prim_ids.size());
    int nodeIndex = 0;
    for(auto& node : bvh.nodes) {
      writeBVHNode(out, node, nodeIndex++);
    }
    for(auto&& prim_id : bvh.prim_ids) {
      out.push_back(prim_id);
    }
  }
}

/**
 * Creates a BVH of all object AABBs
 * The result is a list of 16bit ints encoding both nodes, indices and AABB extends
 * @param modelChunks
 */
std::vector<int16_t> createMeshBVH(const std::vector<ModelChunked> &modelChunks)
{
  std::vector<BBox> aabbs;
  std::vector<BVec3> centers;
  for(auto &chunks : modelChunks)
  {
    aabbs.emplace_back(
      BVec3(chunks.aabbMin[0], chunks.aabbMin[1], chunks.aabbMin[2]),
      BVec3(chunks.aabbMax[0], chunks.aabbMax[1], chunks.aabbMax[2])
    );
    centers.push_back(aabbs.back().get_center());
  }

  bvh::v2::ThreadPool thread_pool;
  typename bvh::v2::DefaultBuilder<Node>::Config config;
  config.quality = bvh::v2::DefaultBuilder<Node>::Quality::High;
  auto bvh = bvh::v2::DefaultBuilder<Node>::build(thread_pool, aabbs, centers, config);

  std::vector<int16_t> treeData;
  writeBVH(treeData, bvh);
  return treeData;
}