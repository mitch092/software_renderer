#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <vector>
#include "Color.h"

void calculate_light(const std::vector<glm::vec3>& normals, const size_t& visible_triangles_size,
                     std::vector<Color>& shades) {
  for (size_t i = 0; i != visible_triangles_size; ++i) {
    float intensity = glm::dot(normals[i], glm::vec3(0, 0, -1));
    // std::cout << intensity << std::endl;
    Uint8 shade = static_cast<Uint8>(intensity * 255);
    shades[i] = Color{shade, shade, shade};
    // std::cout << normals[i].x << " " << normals[i].y << " " << normals[i].z << " " << std::endl;
  }
  // One extra color at the end (black), so that zbuffer_color_id can simply be initialized with shades.size() - 1,
  // So blank pixels are set to black and branching is not needed in order to determine which pixels need to be black.
  // shades.size() - 1 == visible_triangles_size, so I use the simpler constant value.
  assert(shades.size() - 1 == visible_triangles_size);
  shades[visible_triangles_size] = Color{0, 0, 0};
}

float calculate_directional_light(const std::vector<glm::vec3>& directionalLights, const glm::vec3& normal) {
  float intensity = 0.0f;
  for (const glm::vec3& directionalLight : directionalLights) {
    float result = glm::dot(normal, directionalLight);
    if (result > 0.0f) intensity += result;
    if (intensity >= 1.0f) {
      intensity = 1.0f;
      break;
    }
  }
  return intensity;
}