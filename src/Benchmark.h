#pragma once
#include <chrono>

class Stopwatch {
 public:
  Stopwatch() = default;
  ~Stopwatch() = default;

  void reset_and_start() {
	  elapsed = std::chrono::system_clock::now(); 
  }
  void stop_and_print() {
    auto end = std::chrono::system_clock::now();
    std::string time;
    time.append("Elapsed time: ");
    time.append(std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(end - elapsed).count()));
    time.append(" nanoseconds\n");
    std::cerr << time;
  }

 private:
  std::chrono::time_point<std::chrono::system_clock> elapsed;
};