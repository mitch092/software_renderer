#pragma once
#include "Pixel.h"
class DrawLine {
 public:
  DrawLine(int _x0, int _y0, int _x1, int _y1, Pixel _pixel) : x0(_x0), y0(_y0), x1(_x1), y1(_y1), pixel(_pixel) {}

  void operator()(Renderer& renderer) {
    for (float t = 0.0f; t < 1.0f; t += 0.01f) {
      float x = x0 + (x1 - x0) * t;
      float y = y0 + (y1 - y0) * t;
      renderer.put_pixel((int)x, (int)y, pixel);
    }
  }

 private:
  int x0, y0, x1, y1;
  Pixel pixel;
};