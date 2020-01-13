#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Transform.h"
#include "dod/model.h"

struct Scene {
  Scene(const glm::mat4& _proj, int _width, int _height)
      : proj(_proj),
        width(_width),
        height(_height),
        viewport(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height)) {}

  void addModel(Transform _transform, std::string& file) {
    std::vector<glm::vec3> _verts;
    std::vector<glm::uvec3> _faces;
    get_model(file, _verts, _faces);
    models.addTransform(_transform);
    faces.push_back(_faces);
    verts.push_back(_verts);
    transformedVerts.push_back(_verts);
  }
  void addCamera(Transform _camera) { camera = _camera; }
  void addLight(glm::vec3 light) { directionalLights.push_back(light); }

  int width, height;
  glm::vec4 viewport;
  const glm::mat4 proj;

  Transforms models;
  // Outer vector: models, inner vector: faces.
  std::vector<std::vector<glm::uvec3>> faces;
  std::vector<std::vector<glm::vec3>> verts;
  std::vector<std::vector<glm::vec3>> transformedVerts;

  Transform camera;

  std::vector<glm::vec3> directionalLights;
};
