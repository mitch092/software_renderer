#pragma once
#include <memory>
#include "Color.h"
#include "dod/RectangularArray.h"
#include "dod/data.h"
#include "wireframe.h"

class Renderer {
 public:
  Renderer(std::string& file, int width, int height, const glm::mat4& matrix) : data(std::unique_ptr<Data>(new Data{})) {
    init_data(file, width, height, matrix, *data.get());
  }

  void draw_model(const glm::mat4& matrix, RectangularArray<Color>& frame) {
    update_data(matrix, *data.get());
    update_data_model(*data.get(), frame);
  }
  void draw_wireframe(const glm::mat4& matrix, RectangularArray<Color>& frame) {
    update_data(matrix, *data.get());
    draw_wireframe_pixels(data.get()->visible_triangles, frame);
  }

 private:
  std::unique_ptr<Data> data;
};