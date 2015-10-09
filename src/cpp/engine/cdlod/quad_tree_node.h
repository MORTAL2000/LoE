// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_NODE_H_
#define ENGINE_CDLOD_QUAD_TREE_NODE_H_

#include <memory>
#include "./quad_grid_mesh.h"
#include "../global_height_map.h"
#include "../collision/spherized_aabb.h"

namespace engine {
namespace cdlod {

class QuadTreeNode {
 public:
  QuadTreeNode(int x, int z, GLubyte level);

  int size() { return GlobalHeightMap::node_dimension << level_; }

  bool collidesWithSphere(const Sphere& sphere) {
    return bbox_.collidesWithSphere(sphere);
  }

  static bool isVisible(int x, int z, int level);

  void initChildren();

  void selectNodes(const glm::vec3& cam_pos,
                   const Frustum& frustum,
                   QuadGridMesh& grid_mesh);

 private:
  using BBox = SpherizedAABBSat<GlobalHeightMap::geom_w, GlobalHeightMap::geom_h>;

  int x_, z_;
  GLubyte level_;
  BBox bbox_;
  std::unique_ptr<QuadTreeNode> children_[4];
  bool children_inited_ = false;
};

}
}

#endif
