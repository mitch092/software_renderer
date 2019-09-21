#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Primitives.h"
#include "buffers.h"

// I need to optimize everything in this file more!

// struct BarycentricCache {
//  glm::vec3 v0, v1, tria;
//  float invDenom;
//};

// void calculate_bcaches(const std::vector<Triangle>& triangles, const size_t& visible_triangles_size,
//                       std::vector<BarycentricCache>& bcaches) {
//  for (size_t i = 0; i != visible_triangles_size; ++i) {
//    glm::vec3 v0 = triangles[i].b - triangles[i].a;
//    glm::vec3 v1 = triangles[i].c - triangles[i].a;
//    float invDenom = 1.0f / (v0.x * v1.y - v1.x * v0.y);
//    bcaches[i] = BarycentricCache{v0, v1, triangles[i].a, invDenom};
//  }
//}

struct BarycentricCache {
  float v0_x, v0_y, v1_x, v1_y, tria_x, tria_y, invDenom;
};
// Struct of arrays, not array of structs!
struct BarycentricCaches {
  BarycentricCaches() = default;
  BarycentricCaches(size_t i) {
    v0_x = std::vector<float>(i);
    v0_y = std::vector<float>(i);
    v1_x = std::vector<float>(i);
    v1_y = std::vector<float>(i);
    tria_x = std::vector<float>(i);
    tria_y = std::vector<float>(i);
    invDenom = std::vector<float>(i);
  }
  BarycentricCache operator[](const size_t& i) const {
    return BarycentricCache{v0_x[i], v0_y[i], v1_x[i], v1_y[i], tria_x[i], tria_y[i], invDenom[i]};
  }
  std::vector<float> v0_x, v0_y, v1_x, v1_y, tria_x, tria_y, invDenom;
};

void calculate_bcaches(const std::vector<Triangle>& triangles, const size_t& visible_triangles_size,
                       BarycentricCaches& caches, std::vector<float>& temp1) {
  // Load triangles[i].a
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.tria_x[i] = triangles[i].a.x;
  }
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.tria_y[i] = triangles[i].a.y;
  }
  // Load and subtract in order to get v0_x
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.v0_x[i] = triangles[i].b.x;
  }
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.v0_x[i] -= caches.tria_x[i];
  }
  // Load and subtract for v0_y
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.v0_y[i] = triangles[i].b.y;
  }
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.v0_y[i] -= caches.tria_y[i];
  }

  // Load and subtract for v1_x
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.v1_x[i] = triangles[i].c.x;
  }
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.v1_x[i] -= caches.tria_x[i];
  }
  // Load and subtract for v1_y
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.v1_y[i] = triangles[i].c.y;
  }
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.v1_y[i] -= caches.tria_y[i];
  }
  // Calculate and store the first multiplication for invDenom.
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.invDenom[i] = caches.v0_x[i];
  }
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.invDenom[i] *= caches.v1_y[i];
  }
  // Calculate and store (running out of buffers; must use a temporary buffer)
  // For the other multiplication for invDenom.
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    temp1[i] = caches.v1_x[i];
  }
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    temp1[i] *= caches.v0_y[i];
  }
  // Do the one subtraction for invDenom.
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.invDenom[i] -= temp1[i];
  }
  // Do the one division for invDenom.
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    caches.invDenom[i] = 1.0f / caches.invDenom[i];
  }
}

// void barycentric(const BarycentricCache cache, const std::vector<glm::uvec2>& pixel_list,
//                 const size_t& number_of_pixels_for_this_box, std::vector<glm::vec3>& bcoords_per_box,
//                 std::vector<float> temp1, std::vector<float> temp2, std::vector<float> temp3, std::vector<float> temp4,
//                 std::vector<float> temp5) {
//  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
//    glm::vec2 v2 = glm::vec2(pixel_list[i][j]) - glm::vec2(cache.tria);
//
//    float v = (v2.x * cache.v1.y - cache.v1.x * v2.y) * cache.invDenom;
//    float w = (cache.v0.x * v2.y - v2.x * cache.v0.y) * cache.invDenom;
//    float u = 1.0f - v - w;
//    bcoords_per_box[i].emplace_back(u, v, w);
//  }
//}

// v2_x, v2_y, v, w and u are all temporary buffers.
void barycentric(const BarycentricCache cache, const std::vector<glm::uvec2>& pixel_list,
                 const size_t& number_of_pixels_for_this_box, std::vector<glm::vec3>& bcoords_per_box,
                 std::vector<float>& v2_x, std::vector<float>& v2_y, std::vector<float>& v, std::vector<float>& w,
                 std::vector<float>& u) {
  // v2 is loaded and subtracted here.
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    v2_x[number_of_pixels_for_this_box] = pixel_list[number_of_pixels_for_this_box].x;
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    v2_x[number_of_pixels_for_this_box] -= cache.tria_x;
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    v2_y[number_of_pixels_for_this_box] = pixel_list[number_of_pixels_for_this_box].y;
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    v2_y[number_of_pixels_for_this_box] -= cache.tria_y;
  }
  // The first multiplication for v is stored in v.
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    v[number_of_pixels_for_this_box] = v2_x[number_of_pixels_for_this_box];
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    v[number_of_pixels_for_this_box] *= cache.v1_y;
  }
  // The second multiplication for v is stored in w temporarily.
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    w[number_of_pixels_for_this_box] = v2_y[number_of_pixels_for_this_box];
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    w[number_of_pixels_for_this_box] *= cache.v1_x;
  }
  // The subtraction for v is finally done and now w can be used for other things.
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    v[number_of_pixels_for_this_box] -= w[number_of_pixels_for_this_box];
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    v[number_of_pixels_for_this_box] *= cache.invDenom;
  }

  // The first multiplication for w is stored in w.
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    w[number_of_pixels_for_this_box] = v2_y[number_of_pixels_for_this_box];
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    w[number_of_pixels_for_this_box] *= cache.v0_x;
  }
  // The second multiplication for w is stored in u temporarily.
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    u[number_of_pixels_for_this_box] = v2_x[number_of_pixels_for_this_box];
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    u[number_of_pixels_for_this_box] *= cache.v0_y;
  }
  // The subtraction for w is finally done and now u can be used for other things.
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    w[number_of_pixels_for_this_box] -= u[number_of_pixels_for_this_box];
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    w[number_of_pixels_for_this_box] *= cache.invDenom;
  }
  // Now calculate u.
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    u[number_of_pixels_for_this_box] = 1.0f;
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    u[number_of_pixels_for_this_box] -= v[number_of_pixels_for_this_box];
  }
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    u[number_of_pixels_for_this_box] -= w[number_of_pixels_for_this_box];
  }
  // Now aggregate the bcoords.
  for (size_t i = 0; i != number_of_pixels_for_this_box; ++i) {
    bcoords_per_box[number_of_pixels_for_this_box] =
        glm::vec3(u[number_of_pixels_for_this_box], v[number_of_pixels_for_this_box], w[number_of_pixels_for_this_box]);
  }
}