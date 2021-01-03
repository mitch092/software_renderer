#pragma once
#include <optional>
#include <vector>

template <typename T>
class RectangularArray : public std::vector<T> {
 public:
  RectangularArray() = default;
  RectangularArray(size_t width, size_t height, T preset)
      : std::vector<T>(width * height, preset), _width(width), _height(height) {}

  bool set(size_t x, size_t y, T element) {
    if (in_bounds(x, y)) {
      this->operator[](x + y * _width) = element;
      return true;
    } else {
      return false;
    }
  }

  decltype(auto) get(const size_t& x, const size_t& y) const {
    return in_bounds(x, y) ? std::optional(this->operator[](x + y * _width)) : std::nullopt;
  }

  void set_all(T value) { std::fill(this->begin(), this->end(), value); }

  void resize(size_t width, size_t height) {
    _width = width;
    _height = height;
    this->resize(_width * _height);
  }
  decltype(auto) get_width() const { return _width; }
  decltype(auto) get_height() const { return _height; }

  bool in_bounds(const size_t x, const size_t y) const { return (0 <= x && x < get_width() && 0 <= y && y < get_height()); }

 private:
  // Remove height. Not needed.
  typename std::vector<T>::size_type _width, _height;
};
