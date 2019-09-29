#pragma once
#include <SDL.h>
#include "Frames.h"

class Presenter {
 public:
  Presenter(const char* name, unsigned int width, unsigned int height) : window(nullptr), renderer(nullptr) {
    // Propogating errors (w/o exceptions) is such a drag. Just add assertions and keep them in the release builds :/
    int init = SDL_Init(SDL_INIT_VIDEO);
    assert(init >= 0);

    window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    assert(window != nullptr);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    assert(renderer != nullptr);
  }

  ~Presenter() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void present(const Frames& frames, unsigned int frame) {
    for (size_t y = 0; y != frames.get_height(); ++y) {
      for (size_t x = 0; x != frames.get_width(); ++x) {
        Color pixel = frames.get_pixel(x, y, frame);
        SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, pixel.a);
        SDL_RenderDrawPoint(renderer, x, y);
      }
    }
    SDL_RenderPresent(renderer);
  }

 private:
  SDL_Window* window;
  SDL_Renderer* renderer;
};