#pragma once
#include "SDL.h"

enum struct COLOR { red, green, blue, yellow, cyan, magenta, black, white, silver, grey };

struct Color {
  Color(Uint8 _r, Uint8 _g, Uint8 _b) : r(_r), g(_g), b(_b) {}
  Color() : Color(0, 0, 0) {}
  Color(COLOR color) {
    switch (color) {
      case COLOR::red:
        Color{255, 0, 0};
        break;
      case COLOR::green:
        Color{0, 255, 0};
        break;
      case COLOR::blue:
        Color{0, 0, 255};
        break;
      case COLOR::yellow:
        Color{255, 255, 0};
        break;
      case COLOR::cyan:
        Color{0, 255, 255};
        break;
      case COLOR::magenta:
        Color{255, 0, 255};
        break;
      case COLOR::black:
        Color{0, 0, 0};
        break;
      case COLOR::white:
        Color{255, 255, 255};
        break;
      case COLOR::silver:
        Color{192, 192, 192};
        break;
      case COLOR::grey:
        Color{128, 128, 128};
        break;
    }
  }
  Uint8 r, g, b;
};