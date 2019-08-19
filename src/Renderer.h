#pragma once
#include <algorithm>  // For std::min and std::max
#include <vector>
#include "Color.h"
#include "Frame.h"
#include "Model.h"
#include "Primitives.h"
#include "SDL.h"
#include "glm.hpp"

class Renderer {
 public:
  Renderer(Frame& _frame) : frame(_frame) {}

  void clear_screen(const Color& color) {
    for (int y = 0; y != frame.get_height(); ++y) {
      for (int x = 0; x != frame.get_width(); ++x) {
        frame.put_pixel(x, y, color);
      }
    }
  }

  void draw_line(const glm::ivec2& a, const glm::ivec2& b, const Color& color) {
    int x0 = a.x;
    int y0 = a.y;
    int x1 = b.x;
    int y1 = b.y;

    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
      std::swap(x0, y0);
      std::swap(x1, y1);
      steep = true;
    }
    if (x0 > x1) {
      std::swap(x0, x1);
      std::swap(y0, y1);
    }

    const int dx = x1 - x0;
    const int dy = y1 - y0;
    const int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    const int yincr = (y1 > y0 ? 1 : -1);

    if (steep) {
      for (int x = x0; x <= x1; ++x) {
        frame.put_pixel(y, x, color);
        error2 += derror2;
        if (error2 > dx) {
          y += yincr;
          error2 -= dx * 2;
        }
      }
    } else {
      for (int x = x0; x <= x1; ++x) {
        frame.put_pixel(x, y, color);
        error2 += derror2;
        if (error2 > dx) {
          y += yincr;
          error2 -= dx * 2;
        }
      }
    }
  }

  void draw_triangle_outline(const RenderableTriangle& tri, const Color& color) {
    draw_line(tri.a, tri.b, color);
    draw_line(tri.b, tri.c, color);
    draw_line(tri.c, tri.a, color);
  }

  void draw_quad_outline(const Quad& quad, const Color& color) {
    auto bottom_right = glm::ivec2(quad.ur.x, quad.ll.y);
    auto upper_left = glm::ivec2(quad.ll.x, quad.ur.y);
    draw_line(quad.ll, bottom_right, color);
    draw_line(bottom_right, quad.ur, color);
    draw_line(quad.ur, upper_left, color);
    draw_line(upper_left, quad.ll, color);
  }

  void draw_wireframe(Model& model) {
    for (const auto& tri : model.get_renderable_triangles()) {
      draw_triangle_outline(tri, Color{255, 255, 255});
    }
  }

  void draw_triangle(const RenderableTriangle& tri, const Color& color) {
    auto box = find_bounding_box(tri);
    for (int y = box.ll.y; y <= box.ur.y; ++y) {
      for (int x = box.ll.x; x <= box.ur.x; ++x) {
        auto bc_screen = barycentric(tri, glm::ivec2(x, y));
        if (bc_screen.x >= 0 && bc_screen.y >= 0 && bc_screen.z >= 0) frame.put_pixel(x, y, color);
      }
    }
    // For debugging the bounding_box.
    // draw_quad_outline(box, Color{255, 0, 0});
  }

  void draw_flat_rainbow_shaded_model(Model& model) {
    for (const auto& tri : model.get_renderable_triangles()) {
      draw_triangle(tri, Color{(Uint8)(rand() % 255), (Uint8)(rand() % 255), (Uint8)(rand() % 255)});
    }
  }

  void draw_model_lighted(Model& model) {
    auto light_dir = glm::vec3(0, 0, -1);
    auto screen_coord_triangles = model.get_renderable_triangles();
    auto world_coord_triangles = model.get_transformable_triangles();
    for (int i = 0; i != world_coord_triangles.size(); ++i) {
      auto tri1 = screen_coord_triangles[i];
      auto tri2 = world_coord_triangles[i];
      glm::vec3 n = glm::normalize(glm::cross(tri2.c - tri2.a, tri2.b - tri2.a));

      float intensity = glm::dot(n, light_dir);
      if (intensity > 0) {
        Uint8 shade = static_cast<Uint8>(intensity * 255);
        draw_triangle(tri1, Color{shade, shade, shade});
      }
    }
  }

 private:
  Quad find_bounding_box(const RenderableTriangle& tri) {
    // Take care of the edge case in which the triangle is not in the frame at all.
    if (!frame.in_frame(tri.a.x, tri.a.y) && !frame.in_frame(tri.b.x, tri.b.y) && !frame.in_frame(tri.c.x, tri.c.y)) {
      return Quad{glm::ivec2(0, 0), glm::ivec2(-1, -1)};
    }

    // Find the bounding box around the triangle.
    int left = std::min(tri.a.x, std::min(tri.b.x, tri.c.x));
    int right = std::max(tri.a.x, std::max(tri.b.x, tri.c.x));
    int bottom = std::min(tri.a.y, std::min(tri.b.y, tri.c.y));
    int top = std::max(tri.a.y, std::max(tri.b.y, tri.c.y));

    // Now restrict it so that it's only in the frame.
    left = std::max(left, 0);
    right = std::min(right, (frame.get_width() - 1));
    bottom = std::max(bottom, 0);
    top = std::min(top, frame.get_height() - 1);

    return Quad{glm::ivec2(left, bottom), glm::ivec2(right, top)};
  }

  glm::vec3 barycentric(const RenderableTriangle& tri, const glm::ivec2& P) {
    glm::vec3 a = glm::vec3(tri.c.x - tri.a.x, tri.b.x - tri.a.x, tri.a.x - P.x);
    glm::vec3 b = glm::vec3(tri.c.y - tri.a.y, tri.b.y - tri.a.y, tri.a.y - P.y);
    glm::vec3 u = glm::cross(a, b);
    if (std::abs(u.z) < 1) return glm::vec3(-1, 1, 1);
    return glm::vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
  }

  Frame frame;
};