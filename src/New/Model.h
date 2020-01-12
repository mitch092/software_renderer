#pragma once

struct Model {
  Transform transform;
  Mesh* mesh;
  std::vector<glm::vec3> transformedVertices;
};