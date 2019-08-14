#include <iostream>
#include "App.h"



int main(int argc, char* argv[]) {
  try {

    App app("Test window", 100, 100);
    app.display();

  } catch (const std::runtime_error& error) {

    std::cerr << error.what() << std::endl;
    return EXIT_FAILURE;

  }
  return EXIT_SUCCESS;
}