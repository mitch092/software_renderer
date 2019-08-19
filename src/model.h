#pragma once

#include <vector>
#include <tuple>
#include <optional>
#include "Primitives.h"
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

  void apply_matrix_transform(const glm::mat4& matrix);
  std::vector<RenderableTriangle> get_renderable_triangles();
  std::vector<TransformableTriangle> get_transformable_triangles();
};