#pragma once
#include <glm/glm.hpp>

struct Triangle {
  Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) : a(_a), b(_b), c(_c) {}
  Triangle() = default;
  glm::vec3 a;
  glm::vec3 b;
  glm::vec3 c;
};

