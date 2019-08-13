#pragma once
#include "SDL.h"
class Error {
 public:
  void throw_runtime_error(const char* error) { throw std::runtime_error(format_error_string(error)); }

 private:
  std::string format_error_string(const char* error) {
    std::string error_code("Failed to ");
    error_code.append(error);
    error_code.append(". Error: ");
    error_code.append(SDL_GetError());
    error_code.append("\n");
    return error_code;
  }
};