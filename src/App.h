#pragma once
#include <chrono>  // For benchmarking the various draw_line versions.
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <stdexcept>
#include <string>
#include "Benchmark.h"
#include "Error.h"
#include "Frame.h"
#include "Renderer.h"
#include "SDL.h"

class App {
 public:
  App(const char* name, int width, int height, const char* _model)
      : success(false), window(nullptr), screen(nullptr), model(_model), error() {
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
    prepare_model();
    // render();

    bool quit = false;
    SDL_Event e;

    while (quit == false) {
      while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
          quit = true;
        }
      }
      rotate();
      render();
      SDL_UpdateWindowSurface(window);
    }
  }

 private:
  // The model is in units -1 to 1.
  // Add 1 to every component, divide every component by 2, then multiply the x and y
  // coordinates by the size of the screen.
  void prepare_model() {
    // Add 1.
    auto translate = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    // Divide every component by 2.
    auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    // Multiply x and y by width and height. Not sure about z. Need to test with rectangular windows.
    auto scale2 = glm::scale(glm::mat4(1.0f), glm::vec3(screen->w, screen->h, (screen->w + screen->h) / 2));
    model.apply_matrix_transform(scale2 * scale * translate);
  }

  void rotate() {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), 0.8f, glm::vec3(0, 1, 0));
    model.apply_matrix_transform(rotation);
  }

  void render() {
    Stopwatch watch;
    Renderer renderer{Frame{screen}};

    renderer.clear_screen(Color{0, 0, 0});
    // watch.reset_and_start();
    // renderer.draw_wireframe(model);
    // renderer.draw_flat_rainbow_shaded_model(model);
    renderer.draw_model_lighted(model);
    // renderer.draw_triangle(RenderableTriangle{glm::ivec2{100, 100}, glm::ivec2{900, 100}, glm::ivec2{799/2 + 100, 900}},
    // Color{0, 255, 0});
    // watch.stop_and_print();
  }

  bool success;
  SDL_Window* window;
  SDL_Surface* screen;
  Model model;

  Error error;
};