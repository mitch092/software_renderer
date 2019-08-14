#pragma once
#include "Error.h"
#include "Pixel.h"
#include "SDL.h"

class Renderer {
 public:
  Renderer(SDL_Surface* _surface) : surface(_surface), error() {
    if (SDL_MUSTLOCK(surface)) {
      if (SDL_LockSurface(surface) < 0) {
        error.throw_runtime_error("lock the screen");
      }
    }
  }
  ~Renderer() {
    flip_in_place_horizontally(surface);
    if (SDL_MUSTLOCK(surface)) {
      SDL_UnlockSurface(surface);
    }
  }

  Pixel get_pixel(int x, int y) { return get_pixel(surface, x, y); }
  void put_pixel(int x, int y, Pixel _pixel) { return put_pixel(surface, x, y, _pixel); }

 private:
  Pixel get_pixel(SDL_Surface* _surface, int x, int y) {
    int bpp = _surface->format->BytesPerPixel;

    Uint8* p = (Uint8*)_surface->pixels + y * _surface->pitch + x * bpp;

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
    SDL_GetRGB(pixel, _surface->format, &red, &green, &blue);
    return Pixel(red, green, blue);
  }
  void put_pixel(SDL_Surface* _surface, int x, int y, Pixel _pixel) {
    Uint32 pixel = SDL_MapRGB(_surface->format, _pixel.r, _pixel.g, _pixel.b);

    int bpp = _surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)_surface->pixels + y * _surface->pitch + x * bpp;

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

  void swap_pixels(SDL_Surface* _surface, int x0, int y0, int x1, int y1) {
    Pixel pixel0 = get_pixel(_surface, x0, y0);
    put_pixel(_surface, x0, y0, get_pixel(_surface, x1, y1));
    put_pixel(_surface, x1, y1, pixel0);
  }
  void flip_in_place_vertically(SDL_Surface* _surface) {
    for (int y = 0; y != surface->h; ++y) {
      for (int x = 0; x != (surface->w / 2); ++x) {
        swap_pixels(_surface, x, y, surface->w - 1 - x, y);
      }
    }
  }
  void flip_in_place_horizontally(SDL_Surface* _surface) {
    for (int x = 0; x != surface->w; ++x) {
      for (int y = 0; y != (surface->h / 2); ++y) {
        swap_pixels(_surface, x, y, x, surface->h - 1 - y);
      }
    }
  }

  SDL_Surface* surface;

  Error error;
};