#pragma once

#include <vector>
#include "Mesh.h"
#include "Model.h"
#include "Transform.h"

struct Scene {
  std::vector<Mesh> meshes;
  std::vector<Model> models;
  std::vector<Transform> cameras;
  std::vector<glm::vec3> directionalLights;
};
