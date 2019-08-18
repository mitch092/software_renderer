#pragma once
#include <algorithm>  // For std::min and std::max
#include <vector>
#include "Color.h"
#include "Frame.h"
#include "SDL.h"
#include "glm.hpp"
#include "model.h"

class Renderer {
 public:
  Renderer(Frame& _frame) : frame(_frame) {}

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

  void draw_triangle_outline(const glm::ivec2& t0, const glm::ivec2& t1, const glm::ivec2& t2, const Color& color) {
    draw_line(t0, t1, color);
    draw_line(t1, t2, color);
    draw_line(t2, t1, color);
  }

  void draw_wireframe(Model& model) {
    for (int i = 0; i < model.nfaces(); ++i) {
      glm::vec3 v0 = model.vert(model.face(i)[0]);
      glm::vec3 v1 = model.vert(model.face(i)[1]);
      glm::vec3 v2 = model.vert(model.face(i)[2]);

      glm::ivec2 a = vec3_to_ivec2(v0);
      glm::ivec2 b = vec3_to_ivec2(v1);
      glm::ivec2 c = vec3_to_ivec2(v2);

      draw_triangle_outline(a, b, c, Color{255, 255, 255});
    }
  }

  std::pair<glm::ivec2, glm::ivec2> find_bounding_box(const glm::ivec2& t0, const glm::ivec2& t1, const glm::ivec2& t2) {
    // Take care of the edge case in which the triangle is not in the frame at all.
    if (!(frame.in_frame(t0.x, t0.y) && frame.in_frame(t1.x, t1.y) && frame.in_frame(t2.x, t2.y))) {
      return {glm::ivec2(0, 0), glm::ivec2(0, 0)};
    }

    // Find the bounding box around the triangle.
    int left = std::min(t0.x, std::min(t1.x, t2.x));
    int right = std::max(t0.x, std::max(t1.x, t2.x));
    int bottom = std::min(t0.y, std::min(t1.y, t2.y));
    int top = std::max(t0.y, std::max(t1.y, t2.y));

    // Now restrict it so that it's only in the frame.
    left = std::max(left, 0);
    right = std::min(right, (frame.get_width() - 1));
    bottom = std::max(bottom, 0);
    top = std::min(top, frame.get_height() - 1);

    return {glm::ivec2(left, bottom), glm::ivec2(right, top)};
  }

  glm::vec3 barycentric(const glm::ivec2& t0, const glm::ivec2& t1, const glm::ivec2& t2, const glm::ivec2& P) {
    glm::vec3 a = glm::vec3(t2.x - t0.x, t1.x - t0.x, t0.x - P.x);
    glm::vec3 b = glm::vec3(t2.y - t0.y, t1.y - t0.y, t0.y - P.y);
    glm::vec3 u = glm::cross(a, b);
    if (std::abs(u.z) < 1) return glm::vec3(-1, 1, 1);
    return glm::vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
  }

  void draw_triangle(const glm::ivec2& t0, const glm::ivec2& t1, const glm::ivec2& t2, const Color& color) {
    auto box = find_bounding_box(t0, t1, t2);
    for (int y = box.first.y; y <= box.second.y; ++y) {
      for (int x = box.first.x; x <= box.second.x; ++x) {
        auto bc_screen = barycentric(t0, t1, t2, glm::ivec2(x, y));
        if (bc_screen.x >= 0 && bc_screen.y >= 0 && bc_screen.z >= 0) frame.put_pixel(x, y, color);
      }
    }
  }

  void draw_flat_rainbow_shaded_model(Model& model) {
    for (int i = 0; i < model.nfaces(); ++i) {
      glm::vec3 v0 = model.vert(model.face(i)[0]);
      glm::vec3 v1 = model.vert(model.face(i)[1]);
      glm::vec3 v2 = model.vert(model.face(i)[2]);
      
      glm::ivec2 a = vec3_to_ivec2(v0);
      glm::ivec2 b = vec3_to_ivec2(v1);
      glm::ivec2 c = vec3_to_ivec2(v2);

      draw_triangle(a, b, c, Color{(Uint8)(rand() % 255), (Uint8)(rand() % 255), (Uint8)(rand() % 255)});
    }
  }

 private:
  glm::ivec2 vec3_to_ivec2(glm::vec3 vec) {
    // OBJ model vertices are a triplet of vertices, each between -1 and 1.
    // Trim off the z vertex.
    auto vec2 = glm::vec2(vec.x, vec.y);
    // Add 1 so that the vector is now between 0 and 2.
    vec2 += glm::vec2(1.0f, 1.0f);
    // Divide by 2 so that the vector is now between 0 and 1.
    vec2 /= 2.0f;
    // Multiply the coordinates so that they map onto the frame.
    vec2.x *= frame.get_width();
    vec2.y *= frame.get_height();
    return glm::ivec2(vec2);
  }

  Frame frame;
};