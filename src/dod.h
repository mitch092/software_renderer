#pragma once
#include <fstream>
#include <vector>
#include "Color.h"
#include "Primitives.h"
#include "SDL.h"


struct Model {
  std::vector<glm::vec3> verts;
  std::vector<glm::ivec3> faces;
};

std::optional<Model> get_model(const char* filename) {
  std::ifstream in;
  in.open(filename, std::ifstream::in);
  if (in.fail()) return std::nullopt;
  std::string line;
  Model model;
  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;
    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      glm::vec3 v;
      iss >> v.x;
      iss >> v.y;
      iss >> v.z;
      model.verts.push_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      std::vector<int> f;
      int itrash, idx;
      iss >> trash;
      while (iss >> idx >> trash >> itrash >> trash >> itrash) {
        idx--;  // in wavefront obj all indices start at 1, not zero
        f.push_back(idx);
      }
      model.faces.emplace_back(f[0], f[1], f[2]);
    }
  }
  std::cerr << "# v# " << model.verts.size() << " f# " << model.faces.size() << std::endl;
  return std::optional<Model>{model};
}

void apply_matrix_transform(const glm::mat4& matrix, std::vector<glm::vec3>& vertices) {
  auto print = [](glm::vec3 vec3) { std::cerr << vec3.x << "," << vec3.y << "," << vec3.z << std::endl; };
  for (int i = 0; i != vertices.size(); ++i) {
    // print(verts_[i]);
    auto vert4 = matrix * glm::vec4(vertices[i], 1.0f);
    auto vert3 = glm::vec3(vert4);
    // print(vert3);
    vertices[i] = vert3;
  }
}

void expand_triangles(const std::vector<glm::vec3>& vertices, const std::vector<glm::ivec3>& faces,
                      std::vector<Triangle>& triangles) {
  triangles.clear();
  for (int i = 0; i != faces.size(); ++i) {
    triangles.emplace_back(vertices[faces[i].x], vertices[faces[i].y], vertices[faces[i].z]);
  }
}




struct Pixel {
  glm::ivec2 pos;
  Color color;
};

struct BarycentricCache {
  glm::vec3 v0, v1, tria;
  float invDenom;
};

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
  //
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
  shades.clear();
  for (int i = 0; i != normals.size(); ++i) {
    float intensity = glm::dot(normals[i], glm::vec3(0, 0, -1));
    Uint8 shade = static_cast<Uint8>(intensity * 255);
    shades.emplace_back(shade, shade, shade);
  }
  // One extra color at the end (black), so that zbuffer_color_id can simply be initialized with shades.size(), 
  // So blank pixels are set to black and branching is not needed in order to determine which pixels need to be black.
  shades.emplace_back(255, 255, 255);
}

void calculate_bcaches(const std::vector<Triangle>& triangles, std::vector<BarycentricCache>& bcaches) {
  bcaches.clear();
  for (int i = 0; i != triangles.size(); ++i) {
    const auto& tri = triangles[i];
    glm::vec3 v0 = tri.b - tri.a;
    glm::vec3 v1 = tri.c - tri.a;
    float invDenom = 1.0f / (v0.x * v1.y - v1.x * v0.y);
    bcaches.emplace_back(v0, v1, tri.a, invDenom);
  }
}











struct Data {
  // Bounding box data. Every triangle gets a bounding box.
  std::vector<Quad> boxes;
  // And every bounding box gets a list of pixels.
  // I have no idea how to hardcode the number of pixels for each bounding box,
  // so I'll allow myself to use std::vector here.
  // Outer list: triangles. Inner list: pixels.
  // ***ALL PIXELS IN BOX FOR EVERY BOX
  JaggedArray<glm::ivec2> pixel_list;

  // Each pixel in each bounding box gets mapped to a barycentric coordinate.
  // ***ALL BCOORDS IN BOX
  JaggedArray<glm::vec3> bcoords;

  // Now create a list containing only the bcoords inside the triangle.
  // &&&ALL BCOORDS IN TRIANGLE
  JaggedArray<glm::vec3> triangle_bcoords;

  // Now create a list containing only the pixels inside the triangle.
  // &&&ALL PIXEL X, Y COORDS IN TRIANGLE
  JaggedArray<glm::ivec2> triangle_pixels;

  // Now calculate the z coordinates for every pixel inside of every triangle.
  // Every pixel on the screen stores a z index.
  // Index every glm::ivec2 from above, update the Z coord and the color if needed.
  // When it comes time to render: For every entry in this array,
  // if the Z is greater than std::numeric_limits<float>::min(),
  // then take that glm::ivec2 and the color and update the pixel list.
  // &&&ALL PIXEL Z COORDS IN TRIANGLE
  JaggedArray<float> z_values_per_triangle;

  // &&&ALL Z COORDS IN THE SCREEN ZBUFFER
  RectangularArray<float> zbuffer;
  // &&&TRIANGLE/SHADE ARRAY INDEX ID FOR EACH FLOAT IN ZBUFFER
  RectangularArray<int> zbuffer_color_id;

  // Every pixel to be drawn to the screen goes here.
  std::vector<Pixel> pixels;
};