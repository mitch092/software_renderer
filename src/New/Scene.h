#pragma once

#include <vector>
#include "Transform.h"


struct Scene {
  float far, near, fov;
  glm::mat4 proj;

  glm::vec4 viewport;

  std::vector<Transform> modelTransforms;
  // Outer vector: models, inner vector: faces.
  std::vector<std::vector<glm::uvec3>> faces;
  std::vector<std::vector<glm::vec3>> verts;
  std::vector<std::vector<glm::vec3>> transformedVerts;

  std::vector<Transform> cameraTransforms;

  std::vector<glm::vec3> directionalLights;
};
