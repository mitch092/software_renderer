#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "pixel.h"

// template <uint32_t width, uint32_t height>
class Frame {
 public:
  Frame(uint32_t _width, uint32_t _height) { buf.resize(_height, std::vector<Pixel>(_width, Pixel())); }
  Frame() : Frame(640, 480) {}

  Pixel getPixel(uint32_t _width, uint32_t _height) const { return buf[_width][_height]; }
  void putPixel(Pixel pixel, uint32_t _width, uint32_t _height) { buf[_width][_height] = pixel; }

  size_t getWidth() { return buf[0].size(); }
  size_t getHeight() { return buf.size(); }

  std::string renderToPPM(const char* _filename) {
    std::string filename = std::string{_filename} + std::string{".ppm"};
    std::string ppm;
    ppm.append("P3\n");
    ppm.append(std::to_string(getWidth()));
    ppm.append(" ");
    ppm.append(std::to_string(getHeight()));
    ppm.append("\n");
    ppm.append(std::to_string(255));
    ppm.append("\n");
	//TODO: fix this part
    /*
    for (auto y = getHeight() - 1; y != -1; --y) {
      for (auto x = 0; x != getWidth(); ++x) {
        Pixel pixel = buf[x][y];
        ppm.append(std::to_string(pixel.r));
        ppm.append(" ");     
		ppm.append(std::to_string(pixel.g));
        ppm.append(" ");
        ppm.append(std::to_string(pixel.b));
        if (x != getWidth() - 1) ppm.append(" ");
      }
      ppm.append("\n");
    }*/
    return ppm;
  }

 private:
  std::vector<std::vector<Pixel>> buf;
};