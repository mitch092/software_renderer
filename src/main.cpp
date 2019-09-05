#include <iostream>
#include "App.h"
#include "dod/model.h"

int main(int argc, char* argv[]) {
  std::optional<std::string> str = get_model_file_data("african_head.obj");
  if (!str.has_value()) {
    std::cerr << "Could not load the object file." << std::endl;
    return EXIT_FAILURE;
  }

  auto app = App("tinyrenderer in SDL", 800, 800, str.value());
  app.display();

  return EXIT_SUCCESS;
}