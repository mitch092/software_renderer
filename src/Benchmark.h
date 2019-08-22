#pragma once
#include <chrono>

class Stopwatch {
 public:
  Stopwatch() : start(), elapsed_seconds(0.0) {}
  ~Stopwatch() = default;

  double get_elapsed_seconds() { return elapsed_seconds; }

  void reset_and_start() { start = std::chrono::system_clock::now(); }
  void stop_and_print_fps() {
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    elapsed_seconds = duration.count();

    std::string time;
    time.append("Elapsed time: ");
    time.append(std::to_string(1.0 / elapsed_seconds));
    time.append(" frames per second.\n");
    std::cerr << time;
  }

 private:
  std::chrono::time_point<std::chrono::system_clock> start;
  double elapsed_seconds;
};