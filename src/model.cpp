#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Primitives.h"

Model::Model(const char* filename) : verts_(), faces_() {
  std::ifstream in;
  in.open(filename, std::ifstream::in);
  if (in.fail()) return;
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
      // for (int i = 0; i < 3; i++) iss >> v.raw[i];
      verts_.push_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      std::vector<int> f;
      int itrash, idx;
      iss >> trash;
      while (iss >> idx >> trash >> itrash >> trash >> itrash) {
        idx--;  // in wavefront obj all indices start at 1, not zero
        f.push_back(idx);
      }
      faces_.push_back(f);
    }
  }
  std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model() {}

int Model::nverts() { return (int)verts_.size(); }

int Model::nfaces() { return (int)faces_.size(); }

std::vector<int> Model::face(int idx) { return faces_[idx]; }

glm::vec3 Model::vert(int i) { return verts_[i]; }

void Model::apply_matrix_transform(const glm::mat4& matrix) {
  auto print = [](glm::vec3 vec3) { std::cerr << vec3.x << "," << vec3.y << "," << vec3.z << std::endl; };

  for (int i = 0; i != verts_.size(); ++i) {
    //print(verts_[i]);
    auto vert4 = matrix * glm::vec4(verts_[i], 1.0f);
    auto vert3 = glm::vec3(vert4);
    //print(vert3);
    verts_[i] = vert3;
  }
}

std::vector<Triangle> Model::get_triangles() {
  auto triangles = std::vector<Triangle>();
  triangles.reserve(nfaces());

  for (int i = 0; i != nfaces(); ++i) {
    triangles.emplace_back(vert(face(i)[0]), vert(face(i)[1]), vert(face(i)[2]));
  }

  return triangles;
}