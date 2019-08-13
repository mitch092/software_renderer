#pragma once
#include <stdexcept>
#include <string>
#include "SDL.h"

class App {
 public:
  App(const char* name, int width, int height) : success(false), window(nullptr), screen(nullptr) {
    success = SDL_Init(SDL_INIT_VIDEO) >= 0;
    if (success == false) throw_runtime_error("initialize SDL");

    window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr) throw_runtime_error("create the window");

    screen = SDL_GetWindowSurface(window);
    if (screen == nullptr) throw_runtime_error("create the screen");
  }
  ~App() {
    if (success == true) {
      if (screen != nullptr) SDL_FreeSurface(screen);
      if (window != nullptr) SDL_DestroyWindow(window);
      SDL_Quit();
    }
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
  std::string format_error_string(const char* error) {
    std::string error_code("Failed to ");
    error_code.append(error);
    error_code.append(". Error: ");
    error_code.append(SDL_GetError());
    error_code.append("\n");
    return error_code;
  }
  void throw_runtime_error(const char* error) { throw std::runtime_error(format_error_string(error)); }

  bool success;
  SDL_Window* window;
  SDL_Surface* screen;
};