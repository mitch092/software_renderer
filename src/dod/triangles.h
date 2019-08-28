#pragma once
#include <vector>

void expand_triangles(const std::vector<glm::vec3>& vertices, const std::vector<glm::ivec3>& faces,
                      std::vector<Triangle>& triangles) {
  triangles.clear();
  for (int i = 0; i != faces.size(); ++i) {
    triangles.emplace_back(vertices[faces[i].x], vertices[faces[i].y], vertices[faces[i].z]);
  }
}

void calculate_normals(const std::vector<Triangle>& triangles, std::vector<glm::vec3>& normals) {
  for (int i = 0; i != triangles.size(); ++i) {
    const auto& tri = triangles[i];
    normals[i] = glm::normalize(glm::cross(tri.c - tri.a, tri.b - tri.a));
  }
}