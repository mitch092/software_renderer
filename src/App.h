#pragma once
#include <assert.h>
#include <chrono>  // For benchmarking the various draw_line versions.
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <stdexcept>
#include <string>
#include "Benchmark.h"
#include "Frame.h"
#include "Renderer.h"
#include "SDL.h"
#include "transforms.h"

class App {
 public:
  App(const char* name, int width, int height, std::string& file)
      : window(nullptr), screen(nullptr), renderer(file, width, height, center_and_scale(width, height)) {
    // Propogating errors (w/o exceptions) is such a drag. Just add assertions and keep them in the release builds :/
    int init = SDL_Init(SDL_INIT_VIDEO);
    assert(init >= 0);

    window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    assert(window != nullptr);

    screen = SDL_GetWindowSurface(window);
    assert(screen != nullptr);

    assert(screen->h > 0);
    assert(screen->w > 0);
  }
  ~App() {
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void display() {
    assert(screen->w > 0);
    assert(screen->h > 0);

    Stopwatch watch;

    bool quit = false;
    SDL_Event e;

    while (quit == false) {
      watch.reset_and_start();
      while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
          quit = true;
        }
      }
      // Rotate 360 degrees every second.
      renderer.update(rotate(watch.get_elapsed_seconds(), glm::two_pi<float>()));
      renderer.draw(Frame{screen});

      SDL_UpdateWindowSurface(window);
      watch.stop_and_print_fps();
    }
  }

 private:
  SDL_Window* window;
  SDL_Surface* screen;
  Renderer renderer;
};
