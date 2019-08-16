#pragma once
#include <chrono>  // For benchmarking the various draw_line versions.
#include <stdexcept>
#include <string>
#include "Error.h"
#include "Frame.h"
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

  

  void display() {
    //render("assets/african_head.obj");
    render_test();

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
  void render(const char* filename) {
    // Frame locks the SDL surface. The surface is unlocked when
    // the Renderer and the frame go out of scope (which is at the end of this render method).
    Renderer renderer{Frame{screen}};
    Model model(filename);

    auto start = std::chrono::system_clock::now();
    renderer.draw_wireframe(model);
    auto end = std::chrono::system_clock::now();
    std::cerr << "Elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " milliseconds\n";
  }

  void render_test() {
    Renderer renderer{Frame{screen}};
    renderer.draw_triangle(Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80), Color{255, 0, 0});
    renderer.draw_triangle(Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180), Color{255, 255, 255});
    renderer.draw_triangle(Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180), Color{0, 255, 0});
    //renderer.draw_triangle(Vec2i(0, 0), Vec2i(799, 0), Vec2i(799/2, 799), Color{0, 255, 0});

  }


  bool success;
  SDL_Window* window;
  SDL_Surface* screen;

  Error error;
};