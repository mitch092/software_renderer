#pragma once
#include "buffers.h"
#include "glm.hpp"

struct Triangle {
  Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) : a(_a), b(_b), c(_c) {}
  glm::vec3 a;
  glm::vec3 b;
  glm::vec3 c;
};

struct Quad {
  Quad(glm::ivec2 _ll, glm::ivec2 _ur) : ll(_ll), ur(_ur) {}
  // Lower left.
  glm::ivec2 ll;
  // Upper right.
  glm::ivec2 ur;
};

class ZBuffer : public RectangularArray<float> {
 public:
  ZBuffer(size_t width, size_t height) : RectangularArray<float>(width, height, std::numeric_limits<float>::min()) {}
};