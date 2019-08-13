#pragma once
#include <cstdint>

struct Pixel {
  Pixel(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {}
  Pixel() : Pixel(0, 0, 0) {}

  uint8_t r, g, b;
};