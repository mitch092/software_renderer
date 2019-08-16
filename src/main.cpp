#include <iostream>
#include "App.h"

int main(int argc, char* argv[]) {
  try {

    App app("tinyrenderer in SDL", 800, 800);
    app.display();

  } catch (const std::runtime_error& error) {

    std::cerr << error.what() << std::endl;
    return EXIT_FAILURE;

  }
  return EXIT_SUCCESS;
}