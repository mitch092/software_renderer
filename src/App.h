#pragma once
#include <SDL.h>
#include <assert.h>

#include <chrono>  // For benchmarking the various draw_line versions.
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <string>

#include "Benchmark.h"
#include "Color.h"
#include "New/Renderer.h"
#include "New/Scene.h"
#include "Presenter.h"
//#include "Renderer.h"
#include "dod/RectangularArray.h"

class App {
 public:
  App(const char* name, int width, int height, std::string& file)
      : scene{glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f), width, height},
        depthbuffer{RectangularArray<float>(width, height, std::numeric_limits<float>::min())},
        frame{RectangularArray<Color>(width, height, Color())},
        presenter{name, width, height} {
    scene.addModel(
        Transform(glm::vec3{0, 0, 0}, glm::quatLookAt(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)), glm::vec3{1, 1, 1}), file);
    scene.addCamera(
        Transform(glm::vec3(0, 0, 3), glm::quatLookAt(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)), glm::vec3{1, 1, 1}));
    scene.addLight(glm::vec3(0, 0, -1));
  }

  void display() {
    Stopwatch watch;

    bool quit = false;
    SDL_Event e;

    while (quit == false) {
      watch.update();

      while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
          quit = true;
        }
      }

      {
        // Get the orientation of the first model.
        glm::quat& orientation = scene.models.modifyTransform(0).orientation;
        // Rotate it in the positive y direction by 2 pi radians per second.
        orientation = glm::rotate(orientation, watch.get_deltatime() * glm::two_pi<float>(), glm::vec3(0, 1, 0));
      }

      render(scene, frame, depthbuffer);
      presenter(frame);

      std::cerr << watch.get_fps_as_string();
    }
  }

 private:
  Scene scene;
  RectangularArray<float> depthbuffer;
  RectangularArray<Color> frame;
  Presenter presenter;
};
