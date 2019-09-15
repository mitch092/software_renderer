#pragma once
#include <vector>

template <typename T>
class RectangularArray : public std::vector<T> {
 public:
  // RectangularArray() : std::vector<T>(), _width(0), _height(0) {}
  RectangularArray() = default;
  RectangularArray(size_t width, size_t height, T preset)
      : std::vector<T>(width * height, preset), _width(width), _height(height) {}
  T& operator()(size_t x, size_t y) { return this->operator[](x + y * _width); }
  const T& operator()(size_t x, size_t y) const { return this->operator[](x + y * _width); }
  void set_all(T value) { std::fill(this->begin(), this->end(), value); }

  void resize(size_t width, size_t height) {
    _width = width;
    _height = height;
    this->resize(_width * _height);
  }
  size_t get_width() const { return _width; }
  size_t get_height() const { return _height; }

 private:
  size_t _width, _height;
};

template <typename T>
class JaggedArray : public std::vector<std::vector<T>> {
 public:
  JaggedArray() : std::vector<std::vector<T>>() {}
  JaggedArray(size_t width, size_t height, T preset) : std::vector<std::vector<T>>(height, std::vector<T>(width, preset)) {}
  JaggedArray(size_t size) : std::vector<std::vector<T>>(size, std::vector<T>()) {}
};
