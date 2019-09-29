#pragma once
#include <vector>
#include "Color.h"

class Frames {
 public:
  Frames() = default;
  Frames(size_t width, size_t height, size_t frames, Color& preset)
      : pixels(width * height * frames, preset), _width(width), _height(height), _frames(frames) {}

  void put_pixel(size_t x, size_t y, size_t frame, const Color& pixel) {
    if (in_bounds(x, y, frame)) pixels[x + _width * (y + _height * frame)] = pixel;
  }
  Color get_pixel(size_t x, size_t y, size_t frame) const {
    if (in_bounds(x, y, frame))
      return pixels[x + _width * (y + _height * frame)];
    else
      return Color();
  }

  void set_all(Color& value) { std::fill(pixels.begin(), pixels.end(), value); }

  size_t get_width() const { return _width; }
  size_t get_height() const { return _height; }
  size_t get_frames() const { return _frames; }

  bool in_bounds(size_t x, size_t y, size_t z) const {
    return (0 <= x && x < get_width() && 0 <= y && y < get_height() && 0 <= z && z < get_frames());
  }

 private:
  std::vector<Color> pixels;
  size_t _width, _height, _frames;
};