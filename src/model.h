#pragma once

#include <vector>
#include "glm.hpp"

class Model {
 private:
  std::vector<glm::vec3> verts_;
  std::vector<std::vector<int> > faces_;

 public:
  Model(const char *filename);
  ~Model();
  int nverts();
  int nfaces();
  glm::vec3 vert(int i);
  std::vector<int> face(int idx);
};