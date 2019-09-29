#pragma once
#include <vector>
#include "Color.h"

class Frame {
 public:
  Frame() = default;
  Frame(size_t width, size_t height, Color& preset) : pixels(width * height, preset), _width(width), _height(height) {}

  void put_pixel(size_t x, size_t y, const Color& pixel) {
    if (in_bounds(x, y)) pixels[x + _width * y] = pixel;
  }
  Color get_pixel(size_t x, size_t y) const {
    if (in_bounds(x, y))
      return pixels[x + _width * y];
    else
      return Color();
  }

  void set_all(Color& value) { std::fill(pixels.begin(), pixels.end(), value); }

  size_t get_width() const { return _width; }
  size_t get_height() const { return _height; }
  const void* get_data() const { return static_cast<const void*>(pixels.data()); }

  bool in_bounds(size_t x, size_t y) const { return (0 <= x && x < get_width() && 0 <= y && y < get_height()); }

 private:
  std::vector<Color> pixels;
  size_t _width, _height;
};