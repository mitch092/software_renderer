#pragma once
#include <algorithm>
#include "Primitives.h"


void calculate_bounding_boxes(const std::vector<Triangle>& triangles, int width, int height, std::vector<Quad>& boxes) {
  boxes.clear();
  for (int i = 0; i != triangles.size(); ++i) {
    int left = std::min(triangles[i].a.x, std::min(triangles[i].b.x, triangles[i].c.x));
    int right = std::max(triangles[i].a.x, std::max(triangles[i].b.x, triangles[i].c.x));
    int bottom = std::min(triangles[i].a.y, std::min(triangles[i].b.y, triangles[i].c.y));
    int top = std::max(triangles[i].a.y, std::max(triangles[i].b.y, triangles[i].c.y));

    // Now restrict it so that it's only in the frame.
    left = std::max(left, 0);
    right = std::min(right, (width - 1));
    bottom = std::max(bottom, 0);
    top = std::min(top, height - 1);

    boxes.emplace_back(glm::ivec2(left, bottom), glm::ivec2(right, top));
  }
}

void calculate_pixel_list_per_box(const std::vector<Quad>& boxes, JaggedArray<glm::ivec2>& pixel_list) {
  for (int i = 0; i != boxes.size(); ++i) {
    const auto& box = boxes[i];
    auto& pixels_for_box = pixel_list[i];
    pixels_for_box.clear();
    for (int y = box.ll.y; y != box.ur.y; ++y) {
      for (int x = box.ll.x; x != box.ur.x; ++x) {
        pixels_for_box.emplace_back(x, y);
      }
    }
  }
}

void bbox_pixel_and_bcoords_to_triangle(const JaggedArray<glm::ivec2>& pixels_per_box,
                                        const JaggedArray<glm::vec3>& bcoords_per_box,
                                        JaggedArray<glm::vec3>& bcoords_per_triangle,
                                        JaggedArray<glm::ivec2>& pixels_per_triangle) {
  assert(pixels_per_box.size() == bcoords_per_box.size() == bcoords_per_triangle.size() == pixels_per_triangle.size());
  // For each box containing bcoords and pixels:
  for (int i = 0; i != bcoords_per_box.size(); ++i) {
    // Get a list of bcoords/pixels from a single box and clear it.
    bcoords_per_triangle[i].clear();
    pixels_per_triangle[i].clear();
    // Then for each bcoord/pixel in that box, test the bcoord to see if the pixel is in
    // the triangle and put the bcoord+pixel in their respective triangle buffers.
    for (int j = 0; j != bcoords_per_box[i].size(); ++j) {
      if (bcoords_per_box[i][j].x >= 0 && bcoords_per_box[i][j].y >= 0 && bcoords_per_box[i][j].z >= 0) {
        bcoords_per_triangle[i].push_back(bcoords_per_box[i][j]);
        pixels_per_triangle[i].push_back(pixels_per_box[i][j]);
      }
    }
  }
}