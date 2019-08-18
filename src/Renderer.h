#pragma once
#include <algorithm>  // For std::min and std::max
#include <vector>
#include "Color.h"
#include "Frame.h"
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

  void draw_wireframe(Model& model) {
    for (int i = 0; i < model.nfaces(); ++i) {
      std::vector<int> face = model.face(i);
      for (int j = 0; j < 3; ++j) {
        glm::vec3 v0 = model.vert(face[j]);
        glm::vec3 v1 = model.vert(face[(j + 1) % 3]);
        int x0 = (v0.x + 1.0) * frame.get_width() / 2.0;
        int y0 = (v0.y + 1.0) * frame.get_height() / 2.0;
        int x1 = (v1.x + 1.0) * frame.get_width() / 2.0;
        int y1 = (v1.y + 1.0) * frame.get_height() / 2.0;
        draw_line(glm::ivec2(x0, y0), glm::ivec2(x1, y1), Color{255, 255, 255});
      }
    }
  }

  void draw_triangle_v1(const glm::ivec2& t0, const glm::ivec2& t1, const glm::ivec2& t2, const Color& color) {
    draw_line(t0, t1, color);
    draw_line(t1, t2, color);
    draw_line(t2, t0, color);
  }

  void draw_triangle_v2(glm::ivec2 t0, glm::ivec2 t1, glm::ivec2 t2) {
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    draw_line(t0, t1, Color{0, 255, 0});
    draw_line(t1, t2, Color{0, 255, 0});
    draw_line(t2, t0, Color{255, 0, 0});
  }

  void draw_triangle_v3(glm::ivec2 t0, glm::ivec2 t1, glm::ivec2 t2) {
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);

    const int total_height = t2.y - t0.y;
    const int segment_height = t1.y - t0.y + 1;

    for (int y = t0.y; y <= t1.y; ++y) {
      float alpha = (float)(y - t0.y) / total_height;
      float beta = (float)(y - t0.y) / segment_height;
      glm::ivec2 A = glm::vec2(t0) + glm::vec2(t2 - t0) * alpha;
      glm::ivec2 B = glm::vec2(t0) + glm::vec2(t1 - t0) * beta;
      frame.put_pixel(A.x, y, Color{255, 0, 0});
      frame.put_pixel(B.x, y, Color{0, 255, 0});
    }
  }

  void draw_triangle_v4(glm::ivec2 t0, glm::ivec2 t1, glm::ivec2 t2, const Color& color) {
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);

    const int total_height = t2.y - t0.y;
    const int lower_segment_height = t1.y - t0.y + 1;
    const int upper_segment_height = t2.y - t1.y + 1;

    for (int y = t0.y; y <= t1.y; ++y) {
      float alpha = (float)(y - t0.y) / total_height;
      float beta = (float)(y - t0.y) / lower_segment_height;
      glm::ivec2 A = glm::vec2(t0) + glm::vec2(t2 - t0) * alpha;
      glm::ivec2 B = glm::vec2(t0) + glm::vec2(t1 - t0) * beta;
      if (A.x > B.x) std::swap(A, B);
      for (int j = A.x; j < B.x; ++j) {
        frame.put_pixel(j, y, color);
      }
    }

    for (int y = t1.y; y <= t2.y; ++y) {
      float alpha = (float)(y - t0.y) / total_height;
      float beta = (float)(y - t1.y) / upper_segment_height;
      glm::ivec2 A = glm::vec2(t0) + glm::vec2(t2 - t0) * alpha;
      glm::ivec2 B = glm::vec2(t1) + glm::vec2(t2 - t1) * beta;
      if (A.x > B.x) std::swap(A, B);
      for (int j = A.x; j <= B.x; ++j) {
        frame.put_pixel(j, y, color);
      }
    }
  }

  void draw_triangle_v5(glm::ivec2 t0, glm::ivec2 t1, glm::ivec2 t2, const Color& color) {
    if (t0.y == t1.y && t0.y == t2.y) return;
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    const int total_height = t2.y - t0.y;
    for (int i = 0; i < total_height; ++i) {
      bool second_half = i > t1.y - t0.y || t1.y == t0.y;
      int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
      float alpha = (float)i / total_height;
      float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;
      glm::ivec2 A = glm::vec2(t0) + glm::vec2(t2 - t0) * alpha;
      glm::ivec2 B = second_half ? glm::vec2(t1) + glm::vec2(t2 - t1) * beta : glm::vec2(t0) + glm::vec2(t1 - t0) * beta;
      if (A.x > B.x) std::swap(A, B);
      for (int j = A.x; j <= B.x; ++j) {
        frame.put_pixel(j, t0.y + i, color);
      }
    }
  }

  std::pair<glm::ivec2, glm::ivec2> find_bounding_box(const glm::ivec2& t0, const glm::ivec2& t1,
                                                                   const glm::ivec2& t2) {
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

  void draw_triangle_v6(const glm::ivec2& t0, const glm::ivec2& t1, const glm::ivec2& t2, const Color& color) {
    auto box = find_bounding_box(t0, t1, t2);
    for (int y = box.first.y; y <= box.second.y; ++y) {
      for (int x = box.first.x; x <= box.second.x; ++x) {
        auto bc_screen = barycentric(t0, t1, t2, glm::ivec2(x, y));
        if (bc_screen.x >= 0 && bc_screen.y >= 0 && bc_screen.z >= 0) frame.put_pixel(x, y, color);
      }
    }
  }

 private:
  Frame frame;
};