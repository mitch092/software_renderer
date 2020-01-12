#pragma once
#include <SDL.h>
#include <assert.h>
#include <chrono>  // For benchmarking the various draw_line versions.
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <stdexcept>
#include <string>
#include "Benchmark.h"
#include "Frame.h"
#include "Presenter.h"
#include "Renderer.h"
#include "transforms.h"

class App {
 public:
  App(const char* name, int width, int height, std::string& file)
      : presenter{name, width, height},
        renderer{file, width, height, center_and_scale(width, height)},
        frame{width, height, Color()} {

  }
  
  void display() {
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

	  // Renderer draws pixels to Frames and then
	  // Frames is passed into Presenter, which takes care 
	  // of updating the window. 

	  frame.set_all(Color());

      // Rotate 360 degrees every second.
      //renderer.draw_wireframe(rotate(watch.get_elapsed_seconds(), glm::two_pi<float>()), frame);
      renderer.draw_model(rotate(watch.get_elapsed_seconds(), glm::two_pi<float>()), frame);
      presenter.present(frame);


      watch.stop_and_print_fps();
    }
  }

 private:
  Scene scene;
  RectangularArray<Color> framebuffer;
  RectangularArray<Float> depthbuffer;
  Presenter presenter;
};
