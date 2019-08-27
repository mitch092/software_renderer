#pragma once

#include <fstream>
#include <optional>
#include <sstream>
#include <tuple>
#include <vector>
#include "Error.h"
#include "Primitives.h"
#include "glm.hpp"

struct Model {
  static Maybe<Model> get_model(const char* filename) {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return Maybe<Model>();
    std::vector<glm::vec3> verts;
    std::vector<glm::ivec3> faces;
    std::string line;
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
        verts.push_back(v);
      } else if (!line.compare(0, 2, "f ")) {
        std::vector<int> f;
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
    return Maybe<Model>{Model(verts, faces)};
  }

  void apply_matrix_transform(const glm::mat4& matrix) {
    auto print = [](glm::vec3 vec3) { std::cerr << vec3.x << "," << vec3.y << "," << vec3.z << std::endl; };
    for (int i = 0; i != verts.size(); ++i) {
      auto vert4 = matrix * glm::vec4(verts[i], 1.0f);
      auto vert3 = glm::vec3(vert4);
      verts[i] = vert3;
    }
  }
  std::vector<Triangle> get_triangles() {
    std::vector<Triangle> triangles;
    for (int i = 0; i != faces.size(); ++i) {
      triangles.emplace_back(verts[faces[i].x], verts[faces[i].y], verts[faces[i].z]);
    }
    return triangles;
  }

  std::vector<glm::vec3> verts;
  std::vector<glm::ivec3> faces;

  Model(std::vector<glm::vec3> _verts, std::vector<glm::ivec3> _faces) : verts(_verts), faces(_faces) {}
  Model() : verts(), faces() {}
};
