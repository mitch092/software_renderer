#pragma once
#include <vector>
#include "Color.h"
#include "Frame.h"
#include "geometry.h"
#include "model.h"

class Renderer {
 public:
  Renderer(Frame& _frame) : frame(_frame) {}

  // Final version of the line drawing function.
  void draw_line(const Vec2i& a, const Vec2i& b, const Color& color) {
    int x0 = a.raw[0];
    int y0 = a.raw[1];
    int x1 = b.raw[0];
    int y1 = b.raw[1];

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
        Vec3f v0 = model.vert(face[j]);
        Vec3f v1 = model.vert(face[(j + 1) % 3]);
        int x0 = (v0.x + 1.0) * frame.get_width() / 2.0;
        int y0 = (v0.y + 1.0) * frame.get_height() / 2.0;
        int x1 = (v1.x + 1.0) * frame.get_width() / 2.0;
        int y1 = (v1.y + 1.0) * frame.get_height() / 2.0;
        draw_line(Vec2i(x0, y0), Vec2i(x1, y1), Color{255, 255, 255});
      }
    }
  }

  void draw_triangle_outline(const Vec2i& t0, const Vec2i& t1, const Vec2i& t2, const Color& color) {
    draw_line(t0, t1, color);
    draw_line(t1, t2, color);
    draw_line(t2, t0, color);
  }

  void draw_triangle_v1(const Vec2i& t0, const Vec2i& t1, const Vec2i& t2, const Color& color) {
    // draw_triangle_outline(t0, t1, t2, color);
    Vec2i center = (t0 + t1 + t2) * (1.0 / 3.0);

    auto same = [](Vec2i& t0, Vec2i& t1) { return t0.raw[0] == t1.raw[0] && t0.raw[1] == t1.raw[1]; };
    auto incr = [&](Vec2i& from) {
      Vec2i direction = center - from;
      float magnitude = sqrt(pow(direction.raw[0], 2) + pow(direction.raw[1], 2));

      Vec2i normalized_unit = Vec2i(round(direction.raw[0] / magnitude), round(direction.raw[1] / magnitude));
      from = from + normalized_unit;
    };

    Vec2i p0 = t0, p1 = t1, p2 = t2;

    while (!same(p0, center) && !same(p1, center) && !same(p2, center)) {
      draw_triangle_outline(p0, p1, p2, color);
      incr(p0);
      incr(p1);
      incr(p2);
    }
    draw_triangle_outline(p0, p1, p2, color);
  }



 private:
  Frame frame;
};