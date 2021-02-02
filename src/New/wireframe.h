#pragma once
#include "Color.h"
#include "Primitives.h"
#include "dod/RectangularArray.h"

void draw_line(int x0, int y0, int x1, int y1, const Color& pixel, RectangularArray<Color>& frame) {
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
      if (frame.in_bounds(y, x)) {
        frame.set(y, x, pixel);
      }
      error2 += derror2;
      if (error2 > dx) {
        y += yincr;
        error2 -= dx * 2;
      }
    }
  } else {
    for (int x = x0; x <= x1; ++x) {
      if (frame.in_bounds(x, y)) {
        frame.set(x, y, pixel);
      }
      error2 += derror2;
      if (error2 > dx) {
        y += yincr;
        error2 -= dx * 2;
      }
    }
  }
}

void draw_wireframe_triangle(Triangle& triangle, Color color, RectangularArray<Color>& frame) {
  draw_line(triangle.a.x, triangle.a.y, triangle.b.x, triangle.b.y, color, frame);
  draw_line(triangle.b.x, triangle.b.y, triangle.c.x, triangle.c.y, color, frame);
  draw_line(triangle.c.x, triangle.c.y, triangle.a.x, triangle.a.y, color, frame);
}