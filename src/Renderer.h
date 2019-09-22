#pragma once
#include <memory>
#include "Frame.h"
#include "dod/data.h"
#include "wireframe.h"

class Renderer {
 public:
  Renderer(std::string& file, int width, int height, const glm::mat4& matrix) : data(std::unique_ptr<Data>(new Data{})) {
    init_data(file, width, height, matrix, *data.get());
  }

  void update(const glm::mat4& matrix) { update_data(matrix, *data.get()); }
  void draw(Frame& frame) { draw_pixels(data.get()->pixels, frame); }
  void clear(Frame& frame) {
    for (int y = 0; y != frame.get_height(); ++y) {
      for (int x = 0; x != frame.get_width(); ++x) {
        frame.put_pixel(x, y, Color{0, 0, 0});
      }
    }
  }
  void draw_wireframe(Frame& frame) {
    clear(frame);
    wireframe(data.get()->visible_triangles, frame);
  }

 private:
  std::unique_ptr<Data> data;
};