#pragma once
#include "Scene.h"
#include "wireframe.h"

void vertex_shader(const glm::vec4& viewport, const glm::mat4& view, const glm::mat4& proj,
                   const std::vector<glm::mat4>& model, const std::vector<std::vector<glm::vec3>>& verts,
                   std::vector<std::vector<glm::vec3>>& trans) {
  for (int i = 0; i != verts.size(); ++i) {
    for (int j = 0; j != verts[i].size(); ++j) {
      trans[i][j] = glm::project(verts[i][j], view * model[i], proj, viewport);
    }
  }
}

void pixel_shader_wireframe(const std::vector<std::vector<glm::uvec3>>& faces,
                            const std::vector<std::vector<glm::vec3>>& vertices, RectangularArray<Color>& frame) {
  for (int i = 0; i != faces.size(); ++i) {
    for (int j = 0; j != faces[i].size(); ++j) {
      Triangle tri = Triangle{vertices[i][faces[i][j].x], vertices[i][faces[i][j].y], vertices[i][faces[i][j].z]};
      if (triangle_in_frame(tri, frame.get_width(), frame.get_height())) {
        draw_wireframe_triangle(tri, Color{255, 255, 255}, frame);
      }
    }
  }
}

// void pixel_shader_lights() {}

void render(Scene& scene, RectangularArray<Color>& frame) {
  frame.set_all(Color());
  vertex_shader(scene.viewport, scene.camera.getInverseTransformMatrix(), scene.proj, scene.models.getMatrices(),
                scene.verts, scene.transformedVerts);
  pixel_shader_wireframe(scene.faces, scene.transformedVerts, frame);
}
