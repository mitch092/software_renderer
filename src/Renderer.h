#pragma once
#include <cstdint>
#include "SDL.h"

struct Pixel {
  Pixel(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {}
  Pixel() : Pixel(0, 0, 0) {}
  uint8_t r, g, b;
};

class Renderer {
 public:
  render(SDL_Surface* surface) {}

 private:
  Pixel get_pixel(SDL_Surface* surface, uint32_t x, uint32_t y) {
    int bpp = surface->format->BytesPerPixel;
    uint8_t* p = (uint8_t*)surface->pixels + y * surface->pitch + x * bpp;

    uint32_t pixel;

    switch (bpp) {
      case 1:
        pixel = *p;
      case 2:
        pixel = *(uint16_t*)p;
      case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
          pixel = p[0] << 16 | p[1] << 8 | p[2];
        else
          pixel = p[0] | p[1] << 8 | p[2] << 16;
      case 4:
        pixel = *(uint32_t*)p;
      default:
        pixel = 0;
    }

    Pixel pixel2;
    SDL_GetRGB(pixel, surface->format, pixel2.r, pixel2.g, pixel2.b);
    return pixel2;
  }
  void put_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, Pixel pixel) {}
};