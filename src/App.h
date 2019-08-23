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
    Stopwatch watch;
    prepare_model();
    //render();

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
      rotate(watch.get_elapsed_seconds(), glm::two_pi<float>());
      render();

      SDL_UpdateWindowSurface(window);
      watch.stop_and_print_fps();
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

  void rotate(float delta_time, float radians) {
    float angle = radians * delta_time;

    // Save the current coordinates of the model. This is the point to rotate around.
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(400, 400, 400));
    // Apply the inverted coordinates in order to move the model to the origin.
    glm::mat4 invTranslation = glm::inverse(translation);
    // Apply the rotation (around the y axis) with the model on the origin.
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
    // Move the model to the origin, rotate it, then move it back.
    // All of this is for the purpose of rotating the model around its center.
    model.apply_matrix_transform(translation * rotation * invTranslation);
  }

  void render() {
    Renderer renderer{Frame{screen}};
    renderer.clear_screen(Color{0, 0, 0});
    renderer.draw_model_lighted(model);
  }

  bool success;
  SDL_Window* window;
  SDL_Surface* screen;
  Model model;

  Error error;
};