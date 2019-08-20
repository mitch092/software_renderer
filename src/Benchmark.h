#pragma once
#include <chrono>

class Stopwatch {
 public:
  Stopwatch() = default;
  ~Stopwatch() = default;

  void reset_and_start() { elapsed = std::chrono::system_clock::now(); }
  void stop_and_print() {
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - elapsed).count();
    auto duration2 = static_cast<double>(duration);
    duration2 /= 1000000000;

    std::string time;
    time.append("Elapsed time: ");
    time.append(std::to_string(duration2));
    time.append(" seconds\n");
    std::cerr << time;
  }

 private:
  std::chrono::time_point<std::chrono::system_clock> elapsed;
};