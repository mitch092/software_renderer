#pragma once
#include <stdexcept>
#include <string>
#include "Error.h"
#include "Renderer.h"
#include "SDL.h"
#include "DrawLine.h"

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

  App& render(DrawLine& drawline) {
    drawline(Renderer(screen));
    return *this;
  }

  void display() {
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