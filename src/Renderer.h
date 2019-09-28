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

  void draw_model(const glm::mat4& matrix, Frame& frame) {
    update_data(matrix, *data.get());
    update_data_model(*data.get());
    draw_model_pixels(data.get()->pixels, frame);
  }
  void draw_wireframe(const glm::mat4& matrix, Frame& frame) {
    update_data(matrix, *data.get());

    for (int y = 0; y != frame.get_height(); ++y) {
      for (int x = 0; x != frame.get_width(); ++x) {
        frame.put_pixel(x, y, Color{0, 0, 0});
      }
    }

    draw_wireframe_pixels(data.get()->visible_triangles, frame);
  }

 private:
  std::unique_ptr<Data> data;
};