#pragma once

#include <tuple>
#include <vector>
#include <fstream>
#include <sstream>
#include "Error.h"
#include "Primitives.h"
#include "glm.hpp"

struct Model {
  std::vector<glm::vec3> verts;
  std::vector<glm::ivec3> faces;
};

Maybe<Model> get_model(const char* filename) {
  std::ifstream in;
  in.open(filename, std::ifstream::in);
  if (in.fail()) return {};
  std::string line;
  Model model;
  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;
    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      glm::vec3 v;
      iss >> v.x;
      iss >> v.y;
      iss >> v.z;
      // for (int i = 0; i < 3; i++) iss >> v.raw[i];
      model.verts.push_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      std::vector<int> f;
      int itrash, idx;
      iss >> trash;
      while (iss >> idx >> trash >> itrash >> trash >> itrash) {
        idx--;  // in wavefront obj all indices start at 1, not zero
        f.push_back(idx);
      }
      model.faces.emplace_back(f[0], f[1], f[2]);
    }
  }
  std::cerr << "# v# " << model.verts.size() << " f# " << model.faces.size() << std::endl;
  return Maybe{model};
}

void apply_matrix_transform(const glm::mat4& matrix, std::vector<glm::vec3>& vertices) {
  auto print = [](glm::vec3 vec3) { std::cerr << vec3.x << "," << vec3.y << "," << vec3.z << std::endl; };
  for (int i = 0; i != vertices.size(); ++i) {
    // print(verts_[i]);
    auto vert4 = matrix * glm::vec4(vertices[i], 1.0f);
    auto vert3 = glm::vec3(vert4);
    // print(vert3);
    vertices[i] = vert3;
  }
}

void expand_triangles(const std::vector<glm::vec3>& vertices, const std::vector<glm::ivec3>& faces,
                      std::vector<Triangle>& triangles) {
  triangles.clear();
  for (int i = 0; i != faces.size(); ++i) {
    triangles.emplace_back(vertices[faces[i].x], vertices[faces[i].y], vertices[faces[i].z]);
  }
}
