#pragma once
#include <vector>

template <typename T>
class RectangularArray : public std::vector<T> {
 public:
  RectangularArray(size_t width, size_t height, T preset)
      : std::vector<T>(width * height, preset), _width(width), _height(height) {}
  T& operator()(size_t x, size_t y) { return this->operator[](x + y * _width); }
  
 private:
  size_t _width, _height;
};

template <typename T>
class JaggedArray : public std::vector<std::vector<T>> {
 public:
  JaggedArray(size_t width, size_t height, T preset) : std::vector<std::vector<T>>(height, std::vector<T>(width, preset)) {}
};
