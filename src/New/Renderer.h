#pragma once
#include "../dod/barycentric.h"
#include "../dod/box.h"
#include "../dod/lighting.h"
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

void pixel_shader_directional_lights(const std::vector<std::vector<glm::uvec3>>& faces,
                                     const std::vector<std::vector<glm::vec3>>& vertices,
                                     const std::vector<glm::vec3>& directionalLights, RectangularArray<Color>& frame,
                                     RectangularArray<float>& depthbuffer) {
  for (int i = 0; i != faces.size(); ++i) {
    for (int j = 0; j != faces[i].size(); ++j) {
      // Expand a triangle.
      Triangle tri{vertices[i][faces[i][j].x], vertices[i][faces[i][j].y], vertices[i][faces[i][j].z]};
      // Check if the triangle is in the frame.
      if (triangle_in_frame(tri, frame.get_width(), frame.get_height())) {
        // Get its normal vector.

        // There's a bug here.
        glm::vec3 normal = glm::normalize(glm::cross(tri.c - tri.a, tri.b - tri.a));
        // Calculate the shade of the triangle (the color of all of the pixels).
        float intensity = calculate_directional_light(directionalLights, normal);

        Uint8 shade = static_cast<Uint8>(intensity * 255);
        Color triangleShade{shade, shade, shade};
        // Create a barycentric cache.
        BarycentricCache bary{tri};
        glm::uvec4 bbox = calculate_bounding_box(tri, frame.get_width(), frame.get_height());

        // left bottom right top
        // x    y      z     w

        // For every pixel in the bounding box,
        for (auto y = bbox.y; y != bbox.w + 1; ++y) {
          for (auto x = bbox.x; x != bbox.z + 1; ++x) {
            // then calculate the barycentric coordinates.
            glm::vec3 bcoords = bary.calculate_barycentric_coordinates(glm::uvec2{x, y});
            // If the pixel is inside the triangle,
            if (bcoords.x >= 0.0f && bcoords.y >= 0.0f && bcoords.z >= 0.0f) {
              // then calculate the z value for the pixel.
              float z = tri.a.z * bcoords.x + tri.b.z * bcoords.y + tri.c.z * bcoords.z;
              // If the z value is greater than what's currencly in the depth buffer,
              decltype(auto) depthbuffer_z = depthbuffer.get(x, y);
              if (depthbuffer_z.has_value() && z > depthbuffer_z.value()) {
                // then update the depth buffer and the framebuffer.
                depthbuffer.set(x, y, z);
                frame.set(x, y, triangleShade);
              }
            }
          }
        }
      }
    }
  }
}

void render(Scene& scene, RectangularArray<Color>& frame, RectangularArray<float>& depthbuffer) {
  frame.set_all(Color());
  depthbuffer.set_all(std::numeric_limits<float>::min());

  vertex_shader(scene.viewport, scene.camera.getInverseTransformMatrix(), scene.proj, scene.models.getMatrices(),
                scene.verts, scene.transformedVerts);
  // pixel_shader_wireframe(scene.faces, scene.transformedVerts, frame);
  // TODO: Fix bugs.
  pixel_shader_directional_lights(scene.faces, scene.transformedVerts, scene.directionalLights, frame, depthbuffer);
}
