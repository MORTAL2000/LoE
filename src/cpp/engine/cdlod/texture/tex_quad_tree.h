// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_TEXTURE_TEX_QUAD_TREE_H_
#define ENGINE_CDLOD_TEXTURE_TEX_QUAD_TREE_H_

#include <mutex>
#include <memory>
#include <thread>
#include <algorithm>
#include <condition_variable>

#include "./tex_quad_tree_node.h"
#include "../../global_height_map.h"
#include "../../camera.h"
#include "../../oglwrap_all.h"

namespace engine {
namespace cdlod {

class TexQuadTree {
 public:
  TexQuadTree(int w = GlobalHeightMap::tex_w,
              int h = GlobalHeightMap::tex_h,
              glm::ivec2 min_node_size = {256, 128});
  TexQuadTree(int w, int h, GLubyte max_depth);
  ~TexQuadTree();

  glm::ivec2 min_node_size() const { return min_node_size_; }
  TexQuadTreeNode const& root() const { return root_; }
  int max_node_level() const { return max_node_level_; }
  GLuint texture() const { return texture_; }

  void update(Camera const& cam);

 private:
  glm::ivec2 min_node_size_;
  GLubyte max_node_level_;
  TexQuadTreeNode root_;

  size_t load_count_ = 0;
  StreamingInfo streaming_info_;
  GLuint texture_;

  // anync load data
  std::set<TexQuadTreeNode*> load_later_;
  std::mutex load_later_ownership_;
  std::condition_variable condition_variable_;
  std::thread worker_;
  bool worker_should_quit_ = false;
  bool worker_thread_should_sleep_ = true;

  GLubyte max_node_level(int w, int h) const;

  void initTextures();
  void findEmptyPlaces(TexQuadTreeNode* node);
  void imageLoaderThread();
};

}  // namespace cdlod
}  // namespace engine

#endif
