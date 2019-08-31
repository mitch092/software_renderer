#pragma once
#include "Color.h"
#include "SDL.h"

class Frame {
 public:
  Frame(SDL_Surface* _surface) : surface(_surface) {
    if (SDL_MUSTLOCK(surface)) {
      int locked = SDL_LockSurface(surface);
      assert(locked >= 0);
    }
  }
  ~Frame() {
    flip_in_place_horizontally(surface);
    if (SDL_MUSTLOCK(surface)) {
      SDL_UnlockSurface(surface);
    }
  }

  Color get_pixel(int x, int y) { return get_pixel(surface, x, y); }
  void put_pixel(int x, int y, const Color& _color) { 
	  //std::cerr << _color.r << " " << _color.g << " " << _color.b << std::endl;
	  return put_pixel(surface, x, y, _color); }

  int get_height() { return surface->h; }
  int get_width() { return surface->w; }

  bool in_frame(int x, int y) { return (0 <= x && x < get_width() && 0 <= y && y < get_height()); }

 private:
  Color get_pixel(SDL_Surface* _surface, int x, int y) {
    if (!in_frame(x, y)) {
      // std::cerr << "A pixel fell out of bounds: " << x << " " << y << std::endl;
      return Color{0, 0, 0};
    }

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
    return Color(red, green, blue);
  }
  void put_pixel(SDL_Surface* _surface, int x, int y, const Color& _color) {
    if (!in_frame(x, y)) {
      // std::cerr << "A pixel fell out of bounds: " << x << " " << y << std::endl;
      return;
    }

    Uint32 pixel = SDL_MapRGB(_surface->format, _color.r, _color.g, _color.b);

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
    Color color0 = get_pixel(_surface, x0, y0);
    put_pixel(_surface, x0, y0, get_pixel(_surface, x1, y1));
    put_pixel(_surface, x1, y1, color0);
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
};