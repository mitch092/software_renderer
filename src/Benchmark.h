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

    float temp1 = static_cast<float>(currentFrame - lastFrame);
    float temp2 = static_cast<float>(SDL_GetPerformanceFrequency());
    deltatime = temp1 / temp2;
    fps = temp2 / temp1;
  }

  float get_deltatime() { return deltatime; }

  float get_fps() { return fps; }

  std::string get_fps_as_string() {
    std::string message;
    message.append("Elapsed time: ");
    message.append(std::to_string(get_fps()));
    message.append(" frames per second.\n");
    return message;
  }

 private:
  Uint64 lastFrame, currentFrame;
  float deltatime{0.0f}, fps{0.0f};
};