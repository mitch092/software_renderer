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

class App;
using SDL_Success = Either<App, std::string>;

class App {
 public:
  static SDL_Success Init(const char* name, int width, int height, const char* _model) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      return SDL_Success{std::string("SDL_Init() failed.")};
    }

    SDL_Window* window =
        SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
      SDL_Quit();
      return SDL_Success{std::string("SDL_CreateWindow() failed.")};
    }

    SDL_Surface* screen = SDL_GetWindowSurface(window);
    if (screen = nullptr) {
      SDL_DestroyWindow(window);
      SDL_Quit();
      return SDL_Success{std::string("SDL_GetWindowSurface() failed.")};
    }

    auto model = get_model(_model);
    if (model.tag == MAYBE::ABSENT) {
      SDL_DestroyWindow(window);
      SDL_Quit();
      return SDL_Success{std::string("Failed to open the OBJ file.")};
    }

	return SDL_Success{App(window, screen, model.val)};
  }
  ~App() {
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void display() {
    Stopwatch watch;
    prepare_model();
    // render();

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

  // The real constructor is private... no throws here!
  App(SDL_Window* _window, SDL_Surface* _screen, Model& _model) : window(_window), screen(_screen), model(_model) {}

  SDL_Window* window;
  SDL_Surface* screen;
  Model model;
};