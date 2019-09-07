#pragma once
#include <memory>
#include "Frame.h"
#include "dod/data.h"

class Renderer {
 public:
  Renderer(std::string& file, int width, int height, const glm::mat4& matrix) : data(std::unique_ptr<Data>(new Data{})) {
    init_data(file, width, height, matrix, *data.get());
  }

  void update(const glm::mat4& matrix) { update_data(matrix, *data.get()); }
  void draw(Frame& frame) { draw_pixels(data.get()->pixels, frame); }

 private:
  std::unique_ptr<Data> data;
};