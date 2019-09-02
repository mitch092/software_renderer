#include <iostream>
#include "App.h"
#include "dod/model.h"

int main(int argc, char* argv[]) {
  std::optional<std::ifstream> fstream = get_model_file_descriptor("african_head.obj");
  if (!fstream.has_value()) {
    std::cerr << "Could not load the object file." << std::endl;
    return EXIT_FAILURE;
  }

  auto app = App("tinyrenderer in SDL", 800, 800, fstream.value());
  app.display();

  return EXIT_SUCCESS;
}