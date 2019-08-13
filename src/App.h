#pragma once
#include <stdexcept>
#include <string>
#include "Error.h"
#include "Renderer.h"
#include "SDL.h"

class App {
 public:
  App(const char* name, int width, int height) : success(false), window(nullptr), screen(nullptr), error() {
    success = SDL_Init(SDL_INIT_VIDEO) >= 0;
    if (success == false) error.throw_runtime_error("initialize SDL");

    window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr) error.throw_runtime_error("create the window");

    screen = SDL_GetWindowSurface(window);
    if (screen == nullptr) error.throw_runtime_error("create the screen");
  }
  ~App() {
    if (success == true) {
      if (screen != nullptr) SDL_FreeSurface(screen);
      if (window != nullptr) SDL_DestroyWindow(window);
      SDL_Quit();
    }
  }
  // Pass a rendering function in via the main procedure (at compile time).
  // TODO: pull the draw_line function completely out of Renderer and App.
  // TODO: write a function in Renderer that flips the screen after it renders.
  template <typename F>
  App& render(F func) {
    Renderer renderer(screen);
    renderer.draw_line(13, 20, 80, 40, Pixel(255, 255, 255));
    return *this;
  }

  void run() {
    bool quit = false;
    SDL_Event e;

    while (quit == false) {
      while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
          quit = true;
        }
      }
      SDL_UpdateWindowSurface(window);
    }
  }

 private:
  bool success;
  SDL_Window* window;
  SDL_Surface* screen;

  Error error;
};