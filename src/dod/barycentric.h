#pragma once

struct BarycentricCache {
  glm::vec3 v0, v1, tria;
  float invDenom;
};

void calculate_bcaches(const std::vector<Triangle>& triangles, std::vector<BarycentricCache>& bcaches) {
  bcaches.clear();
  for (int i = 0; i != triangles.size(); ++i) {
    const auto& tri = triangles[i];
    glm::vec3 v0 = tri.b - tri.a;
    glm::vec3 v1 = tri.c - tri.a;
    float invDenom = 1.0f / (v0.x * v1.y - v1.x * v0.y);
    bcaches.emplace_back(v0, v1, tri.a, invDenom);
  }
}

void barycentric(const std::vector<BarycentricCache>& bcaches, const JaggedArray<glm::ivec2>& pixel_list,
                 JaggedArray<glm::vec3>& bcoords_per_box) {
  assert(bcaches.size() == pixel_list.size());
  for (int i = 0; i != bcaches.size(); ++i) {
    const auto& cache = bcaches[i];
    const auto& points = pixel_list[i];
    const auto& bcoords = bcoords_per_box[i];
    bcoords.clear();
    for (int j = 0; j != points.size(); ++i) {
      glm::vec3 v2 = glm::vec3(points[j], 0) - cache.tria;
      float v = (v2.x * cache.v1.y - cache.v1.x * v2.y) * cache.invDenom;
      float w = (cache.v0.x * v2.y - v2.x * cache.v0.y) * cache.invDenom;
      float u = 1.0f - v - w;
      bcoords.emplace_back(u, v, w);
    }
  }
}