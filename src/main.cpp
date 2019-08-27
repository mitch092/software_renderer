#include <iostream>
#include <variant>
#include "App.h"

int main(int argc, char* argv[]) {
  Maybe<Model> model = Model::get_model("african_head.obj");
  if (model.tag == MAYBE::ABSENT) {
    std::cerr << "Could not load the object file." << std::endl;
    return EXIT_FAILURE;
  }

  auto app = App("tinyrenderer in SDL", 800, 800, model.val);
  app.display();

  return EXIT_SUCCESS;
}