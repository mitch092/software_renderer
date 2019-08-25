#include <iostream>
#include "App.h"

int main(int argc, char* argv[]) {
  auto app = App::Init("tinyrenderer in SDL", 800, 800, "african_head.obj");
  if (app.tag == EITHER::ERROR) {
    std::cerr << app.val.error << std::endl;
    return EXIT_FAILURE;
  }
  app.val.success.display();

  return EXIT_SUCCESS;
}