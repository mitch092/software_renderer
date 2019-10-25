#pragma once
#include <SDL.h>
#include "Frame.h"

class Presenter {
 public:
  Presenter(const char* name, unsigned int width, unsigned int height) : window(nullptr), renderer(nullptr), texture(nullptr) {
    // Propogating errors (w/o exceptions) is such a drag. Just add assertions and keep them in the release builds :/
    int init = SDL_Init(SDL_INIT_VIDEO);
    assert(init >= 0);

    window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    assert(window != nullptr);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    assert(renderer != nullptr);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    assert(texture != nullptr);
  }

  ~Presenter() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void present(const Frame& frame) {
	// Why 4? Because there are 4 channels. It's a magic number, maybe I can fix it later.
    SDL_UpdateTexture(texture, nullptr, frame.get_data(), frame.get_width() * 4);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
  }

 private:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
};