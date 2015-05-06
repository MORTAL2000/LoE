// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_GLOBAL_HEGIHT_MAP_H_
#define ENGINE_GLOBAL_HEGIHT_MAP_H_

#include <climits>
#include "./transform.h"
#include "./texture_source.h"

namespace engine {

namespace GlobalHeightMap {
 //public:
  extern const char *base_path;
  // geometry division. If this is three, that means that a 8x8 geometry
  // (9x9 vertices) corresponds to a 1x1 texture area (2x2 texels)
  static constexpr long geom_div = 1;
  static constexpr long tex_w = 172800, tex_h = 86400;
  static constexpr long geom_w = tex_w << geom_div;
  static constexpr long geom_h = tex_h << geom_div;
  static constexpr int max_height = 100 << geom_div;
  static constexpr float sphere_radius = geom_w / 2 / M_PI;
};

}  // namespace engine

#endif
