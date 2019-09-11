#pragma once
#include <algorithm>
#include <vector>
#include "Primitives.h"
#include "buffers.h"

void calculate_bounding_boxes(const std::vector<Triangle>& triangles, const int width, const int height,
                              const size_t& visible_triangles_size, std::vector<Quad>& boxes) {
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    int left = std::min(triangles[i].a.x, std::min(triangles[i].b.x, triangles[i].c.x));
    int right = std::max(triangles[i].a.x, std::max(triangles[i].b.x, triangles[i].c.x));
    int bottom = std::min(triangles[i].a.y, std::min(triangles[i].b.y, triangles[i].c.y));
    int top = std::max(triangles[i].a.y, std::max(triangles[i].b.y, triangles[i].c.y));

    // Now restrict it so that it's only in the frame.
    left = std::max(left, 0);
    right = std::min(right, (width - 1));
    bottom = std::max(bottom, 0);
    top = std::min(top, height - 1);

    boxes[i] = Quad{glm::uvec2(left, bottom), glm::uvec2(right, top)};
  }
}

void calculate_pixel_list_per_box(const std::vector<Quad>& boxes, const size_t& visible_triangles_size,
                                  JaggedArray<glm::uvec2>& pixel_list) {
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    pixel_list[i].clear();
    // Holy s***... putting +1 near these for loop conditions fixed all my problems!
    for (size_t y = boxes[i].ll.y; y != boxes[i].ur.y + 1; ++y) {
      for (size_t x = boxes[i].ll.x; x != boxes[i].ur.x + 1; ++x) {
        pixel_list[i].emplace_back(x, y);
      }
    }
  }
}

void bbox_pixel_and_bcoords_to_triangle(const JaggedArray<glm::uvec2>& pixels_per_box,
                                        const JaggedArray<glm::vec3>& bcoords_per_box, const size_t& visible_triangles_size,
                                        JaggedArray<glm::vec3>& bcoords_per_triangle,
                                        JaggedArray<glm::uvec2>& pixels_per_triangle) {
  // For each box containing bcoords and pixels:
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    assert(pixels_per_box[i].size() == bcoords_per_box[i].size());
    assert(bcoords_per_triangle[i].size() == pixels_per_triangle[i].size());

    // Get a list of bcoords/pixels from a single box and clear it.
    bcoords_per_triangle[i].clear();
    pixels_per_triangle[i].clear();
    // Then for each bcoord/pixel in that box, test the bcoord to see if the pixel is in
    // the triangle and put the bcoord+pixel in their respective triangle buffers.
    for (size_t j = 0; j != bcoords_per_box[i].size(); ++j) {
      if (bcoords_per_box[i][j].x >= 0 && bcoords_per_box[i][j].y >= 0 && bcoords_per_box[i][j].z >= 0) {
        bcoords_per_triangle[i].push_back(bcoords_per_box[i][j]);
        pixels_per_triangle[i].push_back(pixels_per_box[i][j]);
      }
    }
  }
}