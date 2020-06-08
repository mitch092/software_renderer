#pragma once
#include <SDL.h>

class Stopwatch {
 public:
  Stopwatch() {
    Uint64 counter = SDL_GetPerformanceCounter();
    lastFrame = counter;
    currentFrame = counter;
  }

  void update() {
    lastFrame = currentFrame;
    currentFrame = SDL_GetPerformanceCounter();
  }

  float get_deltatime() {
    return static_cast<float>(currentFrame - lastFrame) / static_cast<float>(SDL_GetPerformanceFrequency());
  }

  float get_fps() {
    return static_cast<float>(SDL_GetPerformanceFrequency()) / static_cast<float>(currentFrame - lastFrame);
  }

  std::string get_fps_as_string() {
    std::string message;
    message.append("Elapsed time: ");
    message.append(std::to_string(get_fps()));
    message.append(" frames per second.\n");
    return message;
  }

 private:
  Uint64 lastFrame, currentFrame;
};