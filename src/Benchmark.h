#pragma once
#include <SDL.h>

class Stopwatch {
 public:
  Stopwatch() { update_deltatime(); }

  float update_deltatime() {
    currentFrame = SDL_GetPerformanceCounter();
    deltaTime = static_cast<float>(currentFrame - lastFrame) / static_cast<float>(SDL_GetPerformanceFrequency());
    lastFrame = currentFrame;
    return deltaTime;
  }

  void print_fps() {
    std::string time;
    time.append("Elapsed time: ");
    time.append(std::to_string(1.0f / deltaTime));
    time.append(" frames per second.\n");
    std::cerr << time;
  }

 private:
  float deltaTime{0.0f};
  Uint64 lastFrame{0}, currentFrame{0};
};