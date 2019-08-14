#pragma once
#include "Pixel.h"
#include "Frame.h"

class Renderer {
 public:
  Renderer(Frame& _frame) : frame(_frame) {}

  void draw_line1(int x0, int y0, int x1, int y1, Pixel pixel) {
    for (float t = 0.0f; t < 1.0f; t += 0.01f) {
      float x = x0 + (x1 - x0) * t;
      float y = y0 + (y1 - y0) * t;
      frame.put_pixel((int)x, (int)y, pixel);
    }
  }

 private:
  Frame frame;
};