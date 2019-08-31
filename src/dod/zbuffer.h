#pragma once

void calculate_zvalues_per_pixel_per_triangle(const std::vector<Triangle>& triangles,
                                              const JaggedArray<glm::vec3>& triangle_bcoords,
                                              JaggedArray<float>& z_values_per_triangle) {
  assert(triangles.size() == triangle_bcoords.size());
  for (int i = 0; i != triangles.size(); ++i) {
    z_values_per_triangle[i].clear();
    for (int j = 0; j != triangle_bcoords[i].size(); ++j) {
      float z = triangle[i].a.z * triangle_bcoords[i][j].x + triangle[i].b.z * triangle_bcoords[i][j].y +
                triangle[i].c.z * triangle_bcoords[i][j].z;
      z_values_per_triangle[i].push_back(z);
    }
  }
}

void update_zbuffer(const JaggedArray<glm::ivec2>& triangle_pixels, const JaggedArray<float>& z_values_per_triangle,
                    RectangularArray<float>& zbuffer, RectangularArray<int>& zbuffer_color_id) {
  assert(triangle_pixels.size() == z_values_per_triangle.size());
  assert(zbuffer.size() == zbuffer_color_id.size());

  for (int i = 0; i != triangle_pixels.size(); ++i) {
    for (int j = 0; j != triangle_pixels[i].size(); ++j) {
      if (zbuffer(triangle_pixels[i][j].x, triangle_pixels[i][j].y) < z_values_per_triangle[i][j]) {
        zbuffer(triangle_pixels[i][j].x, triangle_pixels[i][j].y) = z_values_per_triangle[i][j];
        zbuffer_color_id(triangle_pixels[i][j].x, triangle_pixels[i][j].y) = i;
      }
    }
  }
}

void update_pixels(const std::vector<Color>& shades, const RectangularArray<int>& zbuffer_color_id,
                   RectangularArray<Color>& pixels) {
  for (int y = 0; y != zbuffer_color_id.get_height(); ++y) {
    for (int x = 0; x != zbuffer_color_id.get_width(); ++x) {
      pixels(x, y) = shades[zbuffer_color_id(x, y)];
    }
  }
}

