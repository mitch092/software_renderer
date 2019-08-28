#pragma once
#include <fstream>
#include <vector>
#include "Color.h"
#include "Primitives.h"
#include "SDL.h"

struct Pixel {
  glm::ivec2 pos;
  Color color;
};



// Here is something crazy: lesson 3's rendering algorithm completely implemented
// using data oriented design. Is this right? Is this wrong?
// Will it run faster? Will it take up more memory? Can it be made more flexible?
// How can I improve this? For now, everything is in an isolated directory
// (because I don't want to modify the other files in place, tbh).

// Other optimizations that I want to make in the future (after getting it to run after the rewrite):
// 1. Test the maximum allocated memory and use a memory pool to preallocate the data for all of the dynamic arrays.
// 2. Implement multithreading. Perhaps it's as simple as adding OpenMP pragmas in the right places.

// When I get this far, I will continue the tinyrenderer tutorial (I leftoff at the UV mapping at the end of lesson 3).
// Rewriting code in this way is easy, but writing new code in the DOD style without knowing what exactly the data in the algorithm is, could be very hard.



struct Data {
  // Get this from model.h
  std::vector<glm::vec3> verts;
  std::vector<glm::ivec3> faces;

  std::vector<Triangle> triangles;
  // Resize this to be the same size as triangles.
  std::vector<glm::vec3> normals;

  // Lighting for each triangle.
  // Resize this to be the same size as triangles.
  std::vector<Color> shades;

  // Calculate whatever I can for barycentric coords using only the Triangles and store it here.
  // Resize this to be the same size as triangles.
  std::vector<BarycentricCache> bcaches;

  // Bounding box data. Every triangle gets a bounding box.
  // TODO
  std::vector<Quad> boxes;
  // And every bounding box gets a list of pixels.
  // Outer list: triangles. Inner list: pixel coordinates (x, y).
  // ***ALL PIXELS IN BOX FOR EVERY BOX
  // TODO
  JaggedArray<glm::ivec2> pixel_list;

  // Each pixel in each bounding box gets mapped to a barycentric coordinate.
  // Outer list: triangles. Inner list: barycentric coordinates.
  // ***ALL BCOORDS IN BOX
  JaggedArray<glm::vec3> bcoords;

  // Now create a list containing only the bcoords inside the triangle.
  // &&&ALL BCOORDS IN TRIANGLE
  // TODO
  JaggedArray<glm::vec3> triangle_bcoords;

  // Now create a list containing only the pixels inside the triangle.
  // &&&ALL PIXEL X, Y COORDS IN TRIANGLE
  // TODO
  JaggedArray<glm::ivec2> triangle_pixels;

  // Now calculate the z coordinates for every pixel inside of every triangle.
  // Every pixel on the screen stores a z index.
  // Index every glm::ivec2 from above, update the Z coord and the color if needed.
  // When it comes time to render: For every entry in this array,
  // if the Z is greater than std::numeric_limits<float>::min(),
  // then take that glm::ivec2 and the color and update the pixel list.
  // &&&ALL PIXEL Z COORDS IN TRIANGLE
  // TODO
  JaggedArray<float> z_values_per_triangle;

  // &&&ALL Z COORDS IN THE SCREEN ZBUFFER
  //TODO
  RectangularArray<float> zbuffer;
  // &&&TRIANGLE/SHADE ARRAY INDEX ID FOR EACH FLOAT IN ZBUFFER
  // TODO
  RectangularArray<int> zbuffer_color_id;

  // Every pixel to be drawn to the screen goes here.
  // TODO
  RectangularArray<Pixel> pixels;
};