#pragma once
#include "glm.hpp"

struct Triangle {
  Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) : a(_a), b(_b), c(_c) {}
  Triangle() = default;
  glm::vec3 a;
  glm::vec3 b;
  glm::vec3 c;
};

struct Quad {
  Quad(glm::uvec2 _ll, glm::uvec2 _ur) : ll(_ll), ur(_ur) {}
  Quad() = default;
  // Lower left.
  glm::uvec2 ll;
  // Upper right.
  glm::uvec2 ur;
};
