#include <iostream>
#include "App.h"
#include "DrawLine.h"



int main(int argc, char* argv[]) {
  DrawLine line(13, 20, 80, 40, Pixel(255, 255, 255));


  try {

    App app("Test window", 100, 100);
    app.render(line).display();

  } catch (const std::runtime_error& error) {

    std::cerr << error.what() << std::endl;
    return EXIT_FAILURE;

  }
  return EXIT_SUCCESS;
}