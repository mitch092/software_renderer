#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Primitives.h"
#include "buffers.h"

void calculate_zvalues_per_pixel_per_triangle(const std::vector<Triangle>& triangles,
                                              const JaggedArray<glm::vec3>& triangle_bcoords,
                                              const size_t& visible_triangles_size,
                                              JaggedArray<float>& z_values_per_triangle) {
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    z_values_per_triangle[i].clear();
    for (size_t j = 0; j != triangle_bcoords[i].size(); ++j) {
      float z = triangles[i].a.z * triangle_bcoords[i][j].x + triangles[i].b.z * triangle_bcoords[i][j].y +
                triangles[i].c.z * triangle_bcoords[i][j].z;
      z_values_per_triangle[i].push_back(z);
    }
  }
}

void update_zbuffer(const JaggedArray<glm::uvec2>& triangle_pixels, const JaggedArray<float>& z_values_per_triangle,
                    const size_t& visible_triangles_size, RectangularArray<float>& zbuffer,
                    RectangularArray<int>& zbuffer_color_id) {
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    for (size_t j = 0; j != triangle_pixels[i].size(); ++j) {
      if (zbuffer(triangle_pixels[i][j].x, triangle_pixels[i][j].y) < z_values_per_triangle[i][j]) {
        zbuffer(triangle_pixels[i][j].x, triangle_pixels[i][j].y) = z_values_per_triangle[i][j];
        zbuffer_color_id(triangle_pixels[i][j].x, triangle_pixels[i][j].y) = i;
      }
    }
  }
}

void update_pixels(const std::vector<Color>& shades, const RectangularArray<int>& zbuffer_color_id,
                   RectangularArray<Color>& pixels) {
  for (size_t y = 0; y != zbuffer_color_id.get_height(); ++y) {
    for (size_t x = 0; x != zbuffer_color_id.get_width(); ++x) {
      pixels(x, y) = shades[zbuffer_color_id(x, y)];
    }
  }
}
