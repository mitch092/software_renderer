#pragma once
#include <chrono>  // For benchmarking the various draw_line versions.
#include <stdexcept>
#include <string>
#include "Error.h"
#include "Frame.h"
#include "Renderer.h"
#include "SDL.h"
#include "Benchmark.h"

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

  void display() {
    render();

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
  void render() {
    Stopwatch watch;
    Renderer renderer{Frame{screen}};

    watch.reset_and_start();
	//Model model(filename);
    //renderer.draw_wireframe(model);
    renderer.draw_triangle_v6(glm::ivec2(10, 70), glm::ivec2(50, 160), glm::ivec2(70, 80), Color{255, 0, 0});
    renderer.draw_triangle_v6(glm::ivec2(180, 50), glm::ivec2(150, 1), glm::ivec2(70, 180), Color{255, 255, 255});
    renderer.draw_triangle_v6(glm::ivec2(180, 150), glm::ivec2(120, 160), glm::ivec2(130, 180), Color{0, 255, 0});
    watch.stop_and_print();
  }

  bool success;
  SDL_Window* window;
  SDL_Surface* screen;

  Error error;
};