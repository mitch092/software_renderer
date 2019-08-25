#pragma once

#include <optional>
#include <tuple>
#include <vector>
#include "Primitives.h"
#include "glm.hpp"

class Model {
 public:
  Model(const char* filename);
  ~Model();
  int nverts();
  int nfaces();
  glm::vec3 vert(int i);
  std::vector<int> face(int idx);

  void apply_matrix_transform(const glm::mat4& matrix);
  std::vector<Triangle> get_triangles();

 private:
  std::vector<glm::vec3> verts_;
  std::vector<std::vector<int> > faces_;
};