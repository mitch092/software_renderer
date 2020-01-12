#pragma once
#include <glm/glm.hpp>
#include <glm/>

class Transform {
 public:
  Transform(glm::vec3 position = glm::vec3(0, 0, 0),
            glm::vec3 orientation = glm::quatLookAt(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),
            glm::vec3 scale = glm::vec3(1, 1, 1))
      : position(position), orientation(orientation), scale(scale) {}

  glm::mat4 getTransformMatrix() { 
	  return glm::translate(position) * glm::mat4_cast(orientation) * glm::scale(scale); 
  }

  glm::mat4 getInverseTransformMatrix() {
    return glm::translate(-1.0f * position) * 
		   glm::mat4_cast(glm::conjugate(orientation)) * 
		   glm::scale(1.0f/scale.x, 1.0f/scale.y, 1.0f/scale.z); 
  }

  glm::vec3 position;
  glm::quat3 orientation;
  glm::vec3 scale;
};