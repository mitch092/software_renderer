#pragma once
#include <glm/ext/quaternion_float.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform {
  Transform(glm::vec3 position = glm::vec3(0, 0, 0),
            glm::quat orientation = glm::quatLookAt(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),
            glm::vec3 scale = glm::vec3(1, 1, 1))
      : position(position), orientation(orientation), scale(scale) {}

  glm::mat4 getTransformMatrix() { return glm::translate(position) * glm::mat4_cast(orientation) * glm::scale(scale); }

  glm::mat4 getInverseTransformMatrix() {
    return glm::translate(-1.0f * position) * glm::mat4_cast(glm::conjugate(orientation)) *
           glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z));
  }

  glm::vec3 position;
  glm::quat orientation;
  glm::vec3 scale;
};

class Transforms {
 public:
  void addTransform(Transform transform) {
    transforms.push_back(transform);
    needsUpdate.push_back(false);
    matrices.push_back(transform.getTransformMatrix());
  }

  Transform& modifyTransform(size_t index) {
    needsUpdate[index] = true;
    return transforms[index];
  }

  std::vector<glm::mat4>& getMatrices() {
    for (int i = 0; i != transforms.size(); ++i) {
      if (needsUpdate[i]) {
        matrices[i] = transforms[i].getTransformMatrix();
        needsUpdate[i] = false;
      }
    }
    return matrices;
  }

 private:
  std::vector<Transform> transforms;
  std::vector<bool> needsUpdate;
  std::vector<glm::mat4> matrices;
};