#pragma once
#include <memory>
#include "Frame.h"
#include "dod/data.h"

class Renderer {
 public:
  Renderer(std::ifstream& file, int width, int height) : data(std::unique_ptr<Data>(new Data{})) {
    init_data(file, width, height, data.get());
  }

  void update(const glm::mat4& matrix) { update(matrix, data.get()); }
  void draw(Frame& frame) { draw_pixels(data.get()->pixels, frame); }

 private:
  std::unique_ptr<Data> data;
};