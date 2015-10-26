// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_TEXTURE_TEX_QUAD_TREE_NODE_H_
#define ENGINE_CDLOD_TEXTURE_TEX_QUAD_TREE_NODE_H_

#include <memory>
#include "../../collision/spherized_aabb.h"

namespace engine {
namespace cdlod {

struct TexQuadTreeNodeIndex {
  GLushort data_offset_hi = 0, data_offset_lo = 0;
  GLushort tex_size_x = 0, tex_size_y = 0;
};

struct DerivativeInfo {
  GLushort dx = 0, dy = 0;
};
static_assert(sizeof(DerivativeInfo) == 2*sizeof(GLushort), "");

class TexQuadTreeNode {
 public:
  TexQuadTreeNode(int center_x, int center_z, int size_x, int size_z,
                  GLubyte mip_level, unsigned index);

  bool collidesWithSphere(const Sphere& sphere) {
    return bbox_.collidesWithSphere(sphere);
  }

  void load();
  void load_files(Magick::Image& height,
                  Magick::Image& dx,
                  Magick::Image& dy) const;
  void load(Magick::Image& height,
            Magick::Image& dx,
            Magick::Image& dy);

  void age();
  void initChild(int i);
  void selectNodes(const glm::vec3& cam_pos,
                   const Frustum& frustum,
                   size_t& last_data_alloc,
                   size_t& uploaded_texel_count,
                   gl::TextureBuffer& height_tex_buffer,
                   gl::TextureBuffer& normal_tex_buffer,
                   gl::TextureBuffer& index_tex_buffer,
                   std::vector<TexQuadTreeNodeIndex>& index_data,
                   std::vector<TexQuadTreeNode*>& data_owners,
                   std::set<TexQuadTreeNode*>& load_later,
                   bool force_load_now);
  void upload(size_t& last_data_alloc,
              size_t& uploaded_texel_count,
              gl::TextureBuffer& height_tex_buffer,
              gl::TextureBuffer& normal_tex_buffer,
              gl::TextureBuffer& index_tex_buffer,
              std::vector<TexQuadTreeNodeIndex>& index_data,
              std::vector<TexQuadTreeNode*>& data_owners);

  int center_x() const { return x_; }
  int center_z() const { return z_; }
  int size_x() const { return sx_; }
  int size_z() const { return sz_; }
  int level() const { return level_; }
  int index() const { return index_; }

  bool is_image_loaded() const { return !height_data_.empty(); }
  int last_used() const { return last_used_; }
  const std::vector<GLushort>& height_data() const { return height_data_; }
  const std::vector<DerivativeInfo>& normal_data() const { return normal_data_; }

  static const int kTimeToLiveOnGPU = 1 << 8;

 private:
  using BBox = SpherizedAABBSat<GlobalHeightMap::tex_w, GlobalHeightMap::tex_h>;

  int x_, z_;
  unsigned sx_, sz_, tex_w_, tex_h_, index_;
  GLubyte level_;
  BBox bbox_;
  std::unique_ptr<TexQuadTreeNode> children_[4];
  std::vector<GLushort> height_data_;
  std::vector<DerivativeInfo> normal_data_;

  int last_used_ = 0;
  // If a node is not used for this much time (frames), it will be unloaded.
  static const int kTimeToLiveInMemory = 1 << 16;
  static_assert(kTimeToLiveOnGPU < kTimeToLiveInMemory, "");
};

}
}

#endif
