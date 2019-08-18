#pragma once
#include "glm.hpp"

// T is ivec2 for render triangles, vec3 for transformable triangles.
template <typename T>
struct Triangle {
  Triangle(T _a, T _b, T _c) : a(_a), b(_b), c(_c) {}
  T a;
  T b;
  T c;
};

using RenderableTriangle = Triangle<glm::ivec2>;
using TransformableTriangle = Triangle<glm::vec3>;

struct Quad {
  Quad(glm::ivec2 _ll, glm::ivec2 _ur) : ll(_ll), ur(_ur) {}
  // Lower left.
  glm::ivec2 ll;
  // Upper right.
  glm::ivec2 ur;
};