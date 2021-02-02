#pragma once
#include <optional>
#include <vector>

template <typename T>
class RectangularArray {
 public:
  using array_size = typename std::vector<T>::size_type;
  RectangularArray() = default;
  RectangularArray(const array_size width, const array_size height, const T preset)
      : buffer(width * height, preset), _width(width), _height(height) {}

  void set(const array_size x, const array_size y, const T element) {
    // Could flip the origin point here, but decided to do it in the projection matrix.
    // y = get_height() - y;
    assert(in_bounds(x, y));
    buffer[x + y * get_width()] = element;
  }

  decltype(auto) get(const array_size x, const array_size y) const {
    assert(in_bounds(x, y));
    return buffer[x + y * get_width()];
  }

  void set_all(const T value) { std::fill(buffer.begin(), buffer.end(), value); }

  void resize(const array_size width, const array_size height) {
    _width = width;
    _height = height;
    buffer.resize(get_width() * get_height());
  }

  decltype(auto) get_width() const { return _width; }
  decltype(auto) get_height() const { return _height; }

  bool in_bounds(const array_size x, const array_size y) const {
    return (0 <= x && x < get_width() && 0 <= y && y < get_height());
  }

  decltype(auto) data() { return buffer.data(); }

 private:
  array_size _width, _height;
  std::vector<T> buffer;
};
