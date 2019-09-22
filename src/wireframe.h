#pragma once
#include <vector>
#include "Color.h"
#include "Frame.h"
#include "Primitives.h"

void draw_line(int x0, int y0, int x1, int y1, const Color& pixel, Frame& frame) {
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
      frame.put_pixel(y, x, pixel);
      error2 += derror2;
      if (error2 > dx) {
        y += yincr;
        error2 -= dx * 2;
      }
    }
  } else {
    for (int x = x0; x <= x1; ++x) {
      frame.put_pixel(x, y, pixel);
      error2 += derror2;
      if (error2 > dx) {
        y += yincr;
        error2 -= dx * 2;
      }
    }
  }
}

void wireframe(std::vector<Triangle>& visible_triangles, Frame& frame) {
  for (int i = 0; i < visible_triangles.size(); ++i) {
    draw_line(visible_triangles[i].a.x, visible_triangles[i].a.y, visible_triangles[i].b.x, visible_triangles[i].b.y,
              Color{255, 255, 255}, frame);
    draw_line(visible_triangles[i].b.x, visible_triangles[i].b.y, visible_triangles[i].c.x, visible_triangles[i].c.y,
              Color{255, 255, 255}, frame);
    draw_line(visible_triangles[i].c.x, visible_triangles[i].c.y, visible_triangles[i].a.x, visible_triangles[i].a.y,
              Color{255, 255, 255}, frame);
  }
}