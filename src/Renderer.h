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

  void draw_model(const glm::mat4& matrix, Frames& frames, unsigned int frame_num) {
    update_data(matrix, *data.get());
    update_data_model(*data.get(), frames, frame_num);
  }
  void draw_wireframe(const glm::mat4& matrix, Frames& frames, unsigned int frame_num) {
    update_data(matrix, *data.get());
    draw_wireframe_pixels(data.get()->visible_triangles, frames, frame_num);
  }

 private:
  std::unique_ptr<Data> data;
};