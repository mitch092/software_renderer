#pragma once
#include <vector>
#include "Frame.h"
#include "Pixel.h"
#include "model.h"


class Renderer {
 public:
  Renderer(Frame& _frame) : frame(_frame) {}

  void draw_line_v1(int x0, int y0, int x1, int y1, const Pixel& pixel) {
    for (float t = 0.0f; t < 1.0f; t += 0.1f) {
      float x = x0 + (x1 - x0) * t;
      float y = y0 + (y1 - y0) * t;
      frame.put_pixel((int)x, (int)y, pixel);
    }
  }
  void draw_line_v2(int x0, int y0, int x1, int y1, const Pixel& pixel) {
    for (int x = x0; x <= x1; ++x) {
      float t = (x - x0) / (float)(x1 - x0);
      int y = y0 * (1.0 - t) + y1 * t;
      frame.put_pixel(x, y, pixel);
    }
  }
  void draw_line_v3(int x0, int y0, int x1, int y1, const Pixel& pixel) {
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
    for (int x = x0; x <= x1; ++x) {
      float t = (x - x0) / (float)(x1 - x0);
      int y = y0 * (1.0 - t) + y1 * t;
      if (steep) {
        frame.put_pixel(y, x, pixel);
      } else {
        frame.put_pixel(x, y, pixel);
      }
    }
  }
  void draw_line_v4(int x0, int y0, int x1, int y1, const Pixel& pixel) {
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

    int dx = x1 - x0;
    int dy = y1 - y0;

    float derror = std::abs(dy / (float)dx);
    float error = 0;

    int y = y0;
    int greater = (y1 > y0 ? 1 : -1);

    for (int x = x0; x <= x1; ++x) {
      if (steep) {
        frame.put_pixel(y, x, pixel);
      } else {
        frame.put_pixel(x, y, pixel);
      }
      error += derror;
      if (error > 0.5) {
        y += greater;
        error -= 1.0;
      }
    }
  }
  void draw_line_v5(int x0, int y0, int x1, int y1, const Pixel& pixel) {
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

    int dx = x1 - x0;
    int dy = y1 - y0;

    int derror2 = std::abs(dy) * 2;
    int error2 = 0;

    int y = y0;
    int greater = (y1 > y0 ? 1 : -1);

    for (int x = x0; x <= x1; ++x) {
      if (steep) {
        frame.put_pixel(y, x, pixel);
      } else {
        frame.put_pixel(x, y, pixel);
      }
      error2 += derror2;
      if (error2 > dx) {
        y += greater;
        error2 -= dx * 2;
      }
    }
  }
  void draw_line_v6(int x0, int y0, int x1, int y1, const Pixel& pixel) {
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

    for (int x = x0; x <= x1; ++x) {
      if (steep) {
        frame.put_pixel(y, x, pixel);
      } else {
        frame.put_pixel(x, y, pixel);
      }
      error2 += derror2;
      if (error2 > dx) {
        y += yincr;
        error2 -= dx * 2;
      }
    }
  }
  void draw_line_v7(int x0, int y0, int x1, int y1, const Pixel& pixel) {
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

  void draw_wireframe(const char* filename) {
    Model model(filename);
    
    for (int i = 0; i < model.nfaces(); ++i) {
      std::vector<int> face = model.face(i);
      for (int j = 0; j < 3; ++j) {
        Vec3f v0 = model.vert(face[j]);
        Vec3f v1 = model.vert(face[(j+1)%3]);
        int x0 = (v0.x + 1.0) * frame.get_width() / 2.0;
        int y0 = (v0.y + 1.0) * frame.get_height() / 2.0;
        int x1 = (v1.x + 1.0) * frame.get_width() / 2.0;
        int y1 = (v1.y + 1.0) * frame.get_height() / 2.0;
        draw_line_v7(x0, y0, x1, y1, Pixel{255, 255, 255});
      }
    }
  }

 private:
  Frame frame;
};