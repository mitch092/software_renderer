#pragma once
#include <vector>
#include "Primitives.h"
#include "buffers.h"
#include "glm.hpp"

struct BarycentricCache {
  glm::vec3 v0, v1, tria;
  float invDenom;
};

void calculate_bcaches(const std::vector<Triangle>& triangles, const size_t& visible_triangles_size,
                       std::vector<BarycentricCache>& bcaches) {
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    glm::vec3 v0 = triangles[i].b - triangles[i].a;
    glm::vec3 v1 = triangles[i].c - triangles[i].a;
    float invDenom = 1.0f / (v0.x * v1.y - v1.x * v0.y);
    bcaches[i] = BarycentricCache{v0, v1, triangles[i].a, invDenom};
  }
}

void barycentric(const std::vector<BarycentricCache>& bcaches, const JaggedArray<glm::uvec2>& pixel_list,
                 const size_t& visible_triangles_size, JaggedArray<glm::vec3>& bcoords_per_box) {
  assert(bcaches.size() != 0);
  assert(pixel_list.size() != 0);
  assert(bcoords_per_box.size() != 0);

  assert(bcaches.size() == pixel_list.size());
  assert(pixel_list.size() == bcoords_per_box.size());

  for (size_t i = 0; i != visible_triangles_size; ++i) {
    const auto& cache = bcaches[i];

    bcoords_per_box[i].clear();

    for (size_t j = 0; j != pixel_list[i].size(); ++j) {
      glm::vec3 v2 = glm::vec3(pixel_list[i][j], 0) - cache.tria;
      float v = (v2.x * cache.v1.y - cache.v1.x * v2.y) * cache.invDenom;
      float w = (cache.v0.x * v2.y - v2.x * cache.v0.y) * cache.invDenom;
      float u = 1.0f - v - w;
      bcoords_per_box[i].emplace_back(u, v, w);
    }
  }
}