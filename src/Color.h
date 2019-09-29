#pragma once
#include <SDL.h>

struct Color {
  Color(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a) : b(_b), g(_g), r(_r), a(_a) {}
  Color(Uint8 _r, Uint8 _g, Uint8 _b) : Color(_r, _g, _b, SDL_ALPHA_OPAQUE) {}
  Color() : Color(0, 0, 0) {}
  Uint8 b, g, r, a;
};