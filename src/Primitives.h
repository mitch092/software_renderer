#pragma once
#include <vector>
#include "glm.hpp"

struct Triangle {
  Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) : a(_a), b(_b), c(_c) {}
  glm::vec3 a;
  glm::vec3 b;
  glm::vec3 c;
};


class ZBuffer : public std::vector<std::vector<float>> {
 public:
  ZBuffer(int width, int height)
      : std::vector<std::vector<float>>(height, std::vector<float>(width, std::numeric_limits<float>::min())) {}
};

struct Quad {
  Quad(glm::ivec2 _ll, glm::ivec2 _ur) : ll(_ll), ur(_ur) {}
  // Lower left.
  glm::ivec2 ll;
  // Upper right.
  glm::ivec2 ur;
};