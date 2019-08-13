#include <iostream>
//#include <glm/glm.hpp>
#include "primitives/frame.h"

int main() {
  Frame frame(3,2);
  frame.putPixel(Pixel(255, 0, 0), 0, 0);
  frame.putPixel(Pixel(0, 255, 0), 0, 1);
  frame.putPixel(Pixel(0, 0, 255), 0, 2);
  frame.putPixel(Pixel(255, 255, 0), 1, 0);
  frame.putPixel(Pixel(255, 255, 255), 1, 1);
  frame.putPixel(Pixel(0, 0, 0), 1, 2);


  std::cout << frame.renderToPPM("C:/Users/steve/Desktop/software_renderer/images");
  return EXIT_SUCCESS;
}