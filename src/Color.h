#pragma once
#include "SDL.h"

struct Color {
  Color(Uint8 _r, Uint8 _g, Uint8 _b) : r(_r), g(_g), b(_b) {}
  Color() : Color(0, 0, 0) {}
  Uint8 r, g, b;
};