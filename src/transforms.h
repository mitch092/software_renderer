#pragma once
#include "glm.hpp"

// The model is in units -1 to 1.
// Add 1 to every component, divide every component by 2, then multiply the x and y
// coordinates by the size of the screen.
glm::mat4 center_and_scale(int width, int height) {  // Add 1.
  auto translate = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
  // Divide every component by 2.
  auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
  // Multiply x and y by width and height. Not sure about z. Need to test with rectangular windows.
  auto scale2 = glm::scale(glm::mat4(1.0f), glm::vec3(width, height, (width + height) / 2));
  return (scale2 * scale * translate);
}

glm::mat4 rotate(float delta_time, float radians) {
  float angle = radians * delta_time;

  // Save the current coordinates of the model. This is the point to rotate around.
  // TODO: 400 400 400 are magic numbers. Fix later on.
  glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(400, 400, 400));
  // Apply the inverted coordinates in order to move the model to the origin.
  glm::mat4 invTranslation = glm::inverse(translation);
  // Apply the rotation (around the y axis) with the model on the origin.
  glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
  // Move the model to the origin, rotate it, then move it back.
  // All of this is for the purpose of rotating the model around its center.
  return (translation * rotation * invTranslation);
}

