#include <iostream>
#include "App.h"

int main(int argc, char* argv[]) {
  try {
    App app("Test window", 640, 480);
    app.run();
  } catch (const std::runtime_error& error) {
    std::cerr << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}