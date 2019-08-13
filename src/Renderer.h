#pragma once
#include "Error.h"
#include "SDL.h"

struct Pixel {
  Pixel(Uint8 _r, Uint8 _g, Uint8 _b) : r(_r), g(_g), b(_b) {}
  Pixel() : Pixel(0, 0, 0) {}
  Uint8 r, g, b;
};

class Renderer {
 public:
  Renderer(SDL_Surface* _surface) : surface(_surface), error() {
    if (SDL_MUSTLOCK(surface)) {
      if (SDL_LockSurface(surface) < 0) {
        error.throw_runtime_error("lock screen");
      }
    }
  }
  ~Renderer() {
    if (SDL_MUSTLOCK(surface)) {
      SDL_UnlockSurface(surface);
    }
  }
  void draw_line(int x0, int y0, int x1, int y1, Pixel pixel) {
    for (float t = 0.0f; t < 1.0f; t += 0.01f) {
      float x = x0 + (x1 - x0) * t;
      float y = y0 + (y1 - y0) * t;
      put_pixel((int)x, (int)y, pixel);
    }
  }

 private:
  Pixel get_pixel(int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    Uint32 pixel;

    switch (bpp) {
      case 1:
        pixel = *p;
        break;
      case 2:
        pixel = *(Uint16*)p;
        break;
      case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
          pixel = p[0] << 16 | p[1] << 8 | p[2];
        else
          pixel = p[0] | p[1] << 8 | p[2] << 16;
        break;
      case 4:
        pixel = *(Uint32*)p;
        break;
      default:
        pixel = 0;
        break;
    }

    Uint8 red, green, blue;
    SDL_GetRGB(pixel, surface->format, &red, &green, &blue);
    return Pixel(red, green, blue);
  }
  void put_pixel(int x, int y, Pixel _pixel) {
    Uint32 pixel = SDL_MapRGB(surface->format, _pixel.r, _pixel.g, _pixel.b);

    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
      case 1:
        *p = pixel;
        break;
      case 2:
        *(Uint16*)p = pixel;
        break;
      case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
          p[0] = (pixel >> 16) & 0xff;
          p[1] = (pixel >> 8) & 0xff;
          p[2] = pixel & 0xff;
        } else {
          p[0] = pixel & 0xff;
          p[1] = (pixel >> 8) & 0xff;
          p[2] = (pixel >> 16) & 0xff;
        }
        break;
      case 4:
        *(Uint32*)p = pixel;
        break;
    }
  }

  SDL_Surface* surface;

  Error error;
};