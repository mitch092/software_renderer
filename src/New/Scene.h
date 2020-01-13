#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Transform.h"

struct Scene {
  Scene(float _far, float _near, float _fov, int _width, int _height)
      : far(_far),
        near(_near),
        fov(_fov),
        width(_width),
        height(_height),
        proj(glm::perspective(fov, (static_cast<float>(width) / static_cast<float>(height)), near, far)) {}

  void addModel(Transform _transform, std::vector<glm::uvec3> _faces, std::vector<glm::vec3> _verts) {
    modelTransforms.push_back(_transform);
    faces.push_back(_faces);
    verts.push_back(_verts);
    transformedVerts.push_back(_verts);
  }
  void addCamera(Transform camera) { cameras.push_back(camera); }
  void addLight(glm::vec3 light) { directionalLights.push_back(light); }

  const float far, near, fov;
  int width, height;
  const glm::mat4 proj;

  std::vector<Transform> modelTransforms;
  // Outer vector: models, inner vector: faces.
  std::vector<std::vector<glm::uvec3>> faces;
  std::vector<std::vector<glm::vec3>> verts;
  std::vector<std::vector<glm::vec3>> transformedVerts;

  std::vector<Transform> cameras;

  std::vector<glm::vec3> directionalLights;
};
