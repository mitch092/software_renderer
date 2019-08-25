#pragma once
#include <fstream>
#include <vector>
#include "Color.h"
#include "Primitives.h"
#include "SDL.h"
#include "model.h"

struct Pixel {
  glm::ivec2 pos;
  Color color;
};

struct Pixel2 {
  float z;
  Color color;
};

struct BarycentricCache {
  glm::vec3 v0, v1, tria;
  float invDenom;
};

BarycentricCache cache_triangle(const Triangle& tri) {
  glm::vec3 v0 = tri.b - tri.a;
  glm::vec3 v1 = tri.c - tri.a;
  float invDenom = 1.0f / (v0.x * v1.y - v1.x * v0.y);
  return {v0, v1, tri.a, invDenom};
}

glm::vec3 barycentric(const BarycentricCache& cache, const glm::ivec2& p) {
  glm::vec3 v2 = glm::vec3(p, 0) - cache.tria;
  float v = (v2.x * cache.v1.y - cache.v1.x * v2.y) * cache.invDenom;
  float w = (cache.v0.x * v2.y - v2.x * cache.v0.y) * cache.invDenom;
  float u = 1.0f - v - w;
  return glm::vec3(u, v, w);
}

// Here is something crazy: lesson 3's rendering algorithm completely implemented
// using data oriented design. Is this right? Is this wrong?
// Will it run faster? Will it take up more memory? Can it be made more flexible?
// How can I improve this? For now, everything is in a single file
// (because I don't want to modify the other files in place, tbh).

struct Data2 {
  // Get this from model.h
  std::vector<Triangle> triangles;
  // Resize this to be the same size as triangles.
  std::vector<glm::vec3> normals;

  // Lighting for each triangle. One light for now, maybe many more later.
  // Resize this to be the same size as triangles.
  std::vector<Color> shades;

  // Calculate whatever I can for barycentric coords using only the Triangles and store it here.
  // Resize this to be the same size as triangles.
  std::vector<BarycentricCache> bcaches;
};

void calculate_normals(const std::vector<Triangle>& triangles, std::vector<glm::vec3>& normals) {
  for (int i = 0; i != triangles.size(); ++i) {
    const auto& tri = triangles[i];
    normals[i] = glm::normalize(glm::cross(tri.c - tri.a, tri.b - tri.a));
  }
}

void calculate_light(const std::vector<glm::vec3>& normals, std::vector<Color>& shades) {
  for (int i = 0; i != normals.size(); ++i) {
    float intensity = glm::dot(normals[i], glm::vec3(0, 0, -1));
    Uint8 shade = static_cast<Uint8>(intensity * 255);
    shades[i] = Color{shade, shade, shade};
  }
}

void calculate_bcaches(const std::vector<Triangle>& triangles, std::vector<BarycentricCache>& bcaches) {
  for (int i = 0; i != triangles.size(); ++i) {
    bcaches[i] = cache_triangle(triangles[i]);
  }
}











struct Data {
  // Bounding box data. Every triangle gets a bounding box.
  std::vector<Quad> boxes;
  // And every bounding box gets a list of pixels.
  // I have no idea how to hardcode the number of pixels for each bounding box,
  // so I'll allow myself to use std::vector here.
  // Outer list: triangles. Inner list: pixels.
  // ***ALL PIXELS IN BOX
  std::vector<std::vector<glm::ivec2>> pixel_list;

  // Each pixel in each bounding box gets mapped to a barycentric coordinate.
  // ***ALL BCOORDS IN BOX
  std::vector<std::vector<glm::vec3>> bcoords;

  // Now create a list containing only the bcoords inside the triangle.
  // &&&ALL BCOORDS IN TRIANGLE
  std::vector<std::vector<glm::vec3>> triangle_bcoords;

  // Now create a list containing only the pixels inside the triangle.
  // &&&ALL PIXEL X, Y COORDS IN TRIANGLE
  std::vector<std::vector<glm::ivec2>> triangle_pixels;

  // Now calculate the z coordinates for every pixel inside of every triangle.
  // &&&ALL PIXEL Z COORDS IN TRIANGLE
  std::vector<std::vector<float>> triangle_z;

  // Every pixel on the screen stores a z index.
  // Index every glm::ivec2 from above, update the Z coord and the color if needed.
  // When it comes time to render: For every entry in this array,
  // if the Z is greater than std::numeric_limits<float>::min(),
  // then take that glm::ivec2 and the color and update the pixel list.
  // Pixel2 zbuffer2[width * height];

  // Outer list: x, Inner list: y
  std::vector<std::vector<Pixel2>> zbuffer;

  // Every pixel to be drawn to the screen goes here.
  std::vector<Pixel> pixels;
};