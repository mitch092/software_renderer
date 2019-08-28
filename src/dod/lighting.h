#pragma once

void calculate_light(const std::vector<glm::vec3>& normals, std::vector<Color>& shades) {
  shades.clear();
  for (int i = 0; i != normals.size(); ++i) {
    float intensity = glm::dot(normals[i], glm::vec3(0, 0, -1));
    Uint8 shade = static_cast<Uint8>(intensity * 255);
    shades.emplace_back(shade, shade, shade);
  }
  // One extra color at the end (black), so that zbuffer_color_id can simply be initialized with shades.size(),
  // So blank pixels are set to black and branching is not needed in order to determine which pixels need to be black.
  shades.emplace_back(255, 255, 255);
}