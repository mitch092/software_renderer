#pragma once
#include "SDL.h"

//enum struct COLOR { red, green, blue, yellow, cyan, magenta, black, white, silver, grey };

struct Color {
  Color(Uint8 _r, Uint8 _g, Uint8 _b) : r(_r), g(_g), b(_b) {}
  Color() : Color(0, 0, 0) {}

  

  Uint8 r, g, b;
};