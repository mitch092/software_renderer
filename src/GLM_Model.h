#pragma once
#include <vector>
#include "Primitives.h"
#include "glm.hpp"
#include "model.h"

class GLM_Model {
 public:
  GLM_Model(Model model) {
    for (int i = 0; i < model.nfaces(); ++i) {
      glm::vec3 v0 = model.vert(model.face(i)[0]);
      glm::vec3 v1 = model.vert(model.face(i)[1]);
      glm::vec3 v2 = model.vert(model.face(i)[2]);

      glm::vec3 a = clean_vec3(v0);
      glm::vec3 b = clean_vec3(v1);
      glm::vec3 c = clean_vec3(v2);

      all_faces.push_back(Triangle(a, b, c));
    }
  }
  std::vector<TransformableTriangle>& get_transformable_triangles() { return all_faces; }

  std::vector<RenderableTriangle> get_renderable_triangles(int width, int height) {
    std::vector<RenderableTriangle> tri;
    for (auto& transformable : get_transformable_triangles()) {
      tri.push_back(transformable_to_renderable(transformable, width, height));
    }
    return tri;
  }

  void print() {
    for (auto& tri : all_faces) {
      std::cerr << tri.a.x << "," << tri.a.y << " " << tri.b.x << "," << tri.b.y << " " << tri.c.x << "," << tri.c.y
                << std::endl;
    }
  }

 private:
  // OBJ vertices come in between -1 and 1. This transformation is done to simplify things.
  glm::vec3 clean_vec3(glm::vec3 vec) {
    // Add 1 so that the vector is now between 0 and 2.
    vec += glm::vec3(1.0f, 1.0f, 1.0f);
    // Divide by 2 so that the vector is now between 0 and 1.
    vec /= 2.0f;
    return vec;
  }

  glm::ivec2 vec3_to_ivec2(glm::vec3 vec, int width, int height) {
    vec.x *= width;
    vec.y *= height;
    return glm::ivec2(vec);
  }

  RenderableTriangle transformable_to_renderable(TransformableTriangle tri, int width, int height) {
    glm::ivec2 a = vec3_to_ivec2(tri.a, width, height);
    glm::ivec2 b = vec3_to_ivec2(tri.b, width, height);
    glm::ivec2 c = vec3_to_ivec2(tri.c, width, height);
    return RenderableTriangle(a, b, c);
  }

  std::vector<TransformableTriangle> all_faces;
};