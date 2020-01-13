#pragma once
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "Primitives.h"

// Make sure the file can be opened.
// Keep this separated from the act of reading the file.
// Isolating the parts of the program that can fail seems like a good idea to me.
std::optional<std::string> get_model_file_data(const char* filename) {
  std::ifstream in(filename, std::ifstream::in);
  if (!in.fail()) {
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    std::string buffer(size, ' ');
    in.seekg(0);
    in.read(&buffer[0], size);
    return {buffer};
  } else {
    return std::nullopt;
  }
}

// Now that we have a valid file handle, write it into buffers just like everything else.
void get_model(std::string& _file, std::vector<glm::vec3>& verts, std::vector<glm::uvec3>& faces) {
  std::istringstream file(_file);

  verts.clear();
  faces.clear();
  std::string line;
  while (!file.eof()) {
    std::getline(file, line);
    std::istringstream iss(line);
    char trash;
    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      glm::vec3 v;
      iss >> v.x;
      iss >> v.y;
      iss >> v.z;
      verts.push_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      std::vector<unsigned int> f;
      int itrash, idx;
      iss >> trash;
      while (iss >> idx >> trash >> itrash >> trash >> itrash) {
        idx--;  // in wavefront obj all indices start at 1, not zero
        f.push_back(idx);
      }
      faces.emplace_back(f[0], f[1], f[2]);
    }
  }
  std::cerr << "# v# " << verts.size() << " f# " << faces.size() << std::endl;
}

void apply_matrix_transform(const glm::mat4& matrix, std::vector<glm::vec3>& verts) {
  for (size_t i = 0; i != verts.size(); ++i) {
    auto vert4 = matrix * glm::vec4(verts[i], 1.0f);
    verts[i] = glm::vec3(vert4);
  }
}

// Checking if a triangle is in the frame is pretty verbose if I expand it out by hand,
// So I'm keeping it factored out of expand_triangles() and using inlines to hopefully
// get the same result as expanding it out by hand.
inline bool pixel_in_frame(int x, int y, int width, int height) { return (0 <= x && x < width) && (0 <= y && y < height); }

// If any of the 2D vertices of the triangle are in the frame, then keep the triangle.
inline bool triangle_in_frame(const Triangle& tri, int width, int height) {
  return pixel_in_frame(tri.a.x, tri.a.y, width, height) || pixel_in_frame(tri.b.x, tri.b.y, width, height) ||
         pixel_in_frame(tri.c.x, tri.c.y, width, height);
}

// Called once every frame. Finds every triangle.
void expand_triangles(const std::vector<glm::vec3>& vertices, const std::vector<glm::uvec3>& faces,
                      std::vector<Triangle>& triangles) {
  for (size_t i = 0; i != faces.size(); ++i) {
    triangles[i] = Triangle{vertices[faces[i].x], vertices[faces[i].y], vertices[faces[i].z]};
  }
}

// Called once. Finds every normal for every triangle.
void initialize_normals(const std::vector<Triangle>& triangles, std::vector<glm::vec3>& normals) {
  for (size_t i = 0; i != triangles.size(); ++i) {
    normals[i] = glm::cross(triangles[i].c - triangles[i].a, triangles[i].b - triangles[i].a);
  }
  for (size_t i = 0; i != triangles.size(); ++i) {
    normals[i] = glm::normalize(normals[i]);
  }
}

// Called every frame, after initialize_normals figures out the normals for the first time.
void update_normals(const glm::mat4& matrix, std::vector<glm::vec3>& normals) {
  const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(matrix)));
  for (size_t i = 0; i != normals.size(); ++i) {
    normals[i] = normalMatrix * normals[i];
  }
  // Must normalize every normal, otherwise they will all be extremely close to zero.
  for (size_t i = 0; i != normals.size(); ++i) {
    normals[i] = glm::normalize(normals[i]);
  }
}

void remove_invisible_triangles_and_normals(const std::vector<Triangle>& all_triangles,
                                            const std::vector<glm::vec3>& all_normals, const int width, const int height,
                                            size_t& visible_triangles_size, std::vector<Triangle>& visible_triangles,
                                            std::vector<glm::vec3>& visible_normals) {
  // Keeps track of which index we are overwriting in visible_triangles.
  visible_triangles_size = 0;

  for (size_t i = 0; i != all_triangles.size(); ++i) {
    if (triangle_in_frame(all_triangles[i], width, height)) {
      visible_triangles[visible_triangles_size] = all_triangles[i];
      visible_normals[visible_triangles_size] = all_normals[i];
      ++visible_triangles_size;
    }
  }
  // When we are done using visible_triangles_size as an arrey indexer, it will contain the
  // actual size of the visible_triangles array.
}