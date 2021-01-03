#include <iostream>

#include "App.h"
#include "dod/model.h"

int main(int argc, char* argv[]) {
  std::string str = get_model_file_data("african_head.obj");

  auto app = App("tinyrenderer in SDL", 800, 800, str);
  app.display();

  return EXIT_SUCCESS;
}