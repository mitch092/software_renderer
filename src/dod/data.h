#pragma once
#include "Color.h"
#include "Frame.h"
#include "Primitives.h"
#include "barycentric.h"
#include "box.h"
#include "buffers.h"
#include "lighting.h"
#include "model.h"
#include "transforms.h"
#include "zbuffer.h"

// Here is something crazy: lesson 3's rendering algorithm completely implemented
// using data oriented design. Is this right? Is this wrong?
// Will it run faster? Will it take up more memory? Can it be made more flexible?
// How can I improve this? For now, everything is in an isolated directory
// (because I don't want to modify the other files in place, tbh).

// Other optimizations that I want to make in the future (after getting it to run after the rewrite):
// 1. Test the maximum allocated memory and use a memory pool to preallocate the data for all of the dynamic arrays.
// 2. Implement multithreading. Perhaps it's as simple as adding OpenMP pragmas in the right places.

// When I get this far, I will continue the tinyrenderer tutorial (I leftoff at the UV mapping at the end of lesson 3).
// Rewriting code in this way is easy, but writing new code in the DOD style without knowing what exactly the data in the
// algorithm is, could be very hard.

struct Data {
  // Get this from frame.h
  int width, height;

  // temporary buffers for calculations.
  std::vector<float> temp1, temp2, temp3, temp4, temp5;

  // Get this from model.h
  // Transform matrices are applied to verts.
  std::vector<glm::vec3> verts;
  std::vector<glm::uvec3> faces;

  // Expand the triangles and ignore triangles that are not in the frame.
  // It's possible that this buffer can change size with each frame
  // (because the model might move in or out of the frame),
  // so I use clear() and push_back()/emplace_back() everywhere.
  // This is not ideal, but I don't know what to do with bounding boxes that are
  // out of the frame. By ignoring the out of frame triangles here, I don't need to worry about that.
  // This also means that the normals, shades and pretty much everything else will only
  // work on viewable triangles, which is a desirable optimization.

  // Verts and faces are expanded every frame to this.
  std::vector<Triangle> all_triangles;
  // The normals are calculated once, when the model is loaded,
  // but then they are updated alongside verts every frame.
  // Transform matrices are also applied here, for normals.
  std::vector<glm::vec3> all_normals;

  // Filter out the triangles that are out of frame.
  std::vector<Triangle> visible_triangles;
  std::vector<glm::vec3> visible_normals;

  // Instead of calling resize every frame for every buffer, I call resize once for every
  // buffer and make them all the same size as all_triangles. But during the filtering
  // of visible_triangles, I update this variable and use this variable as the size
  // argument for every function and piece of data that comes after visible_triangles in this struct.
  size_t visible_triangles_size;

  // Lighting for each triangle.
  // Resize this to be the same size as triangles.
  std::vector<Color> shades;

  // Calculate whatever I can for barycentric coords using only the Triangles and store it here.
  // Resize this to be the same size as triangles.
  BarycentricCaches bcaches;

  // Bounding box data. Every triangle gets a bounding box.
  // Don't need to take care of invisible triangles, as the check is done during the filtering of visible_triangles.
  std::vector<Quad> boxes;

  // Change all JaggedArrays to std::vector. That means that
  // I can make all of the vectors 1920*1080 in size, but
  // I'll need to make a sub-pipline that updates all of the data from pixel_list
  // to zbuffer_color_id and repeats FOR EVERY TRIANGLE. Right now,
  // I update all of the barycentric coords for every triangle before filtering
  // and getting the z-values. So I can no longer reuse the icache for the bcoord
  // calculation between triangles. But reusing it for the coords in a triangle
  // is good enough, and I want to get rid of these big, inefficient JaggedArrays.

  // If we have 2000 triangles and a screen of size 1920x1080,
  // then the highest potential size of a single JaggedArray (bcoords for example)
  // would be:
  // 2000 * 1920 * 1080 * 3 floats * 4 bytes per float = 49 gigabytes.
  // But if I flatten the arrays at the cost of a few extra cache misses,
  // then a single buffer (again, lets say the bcoords buffer)
  // would only be size:
  // 1 * 1920 * 1080 * 3 floats * 4 bytes per float = 25 megabytes.
  // And if I have less than 10 of these buffers, then my program will probably use less than
  // 25 megabytes * 10 = 250 megabytes
  // which is completely reasonable.

  size_t number_of_pixels_for_this_box;
  // And every bounding box gets a list of pixels.
  // Outer list: triangles. Inner list: pixel coordinates (x, y).
  // ***ALL PIXELS IN BOX FOR EVERY BOX
  // ***
  std::vector<glm::uvec2> pixel_list;

  // Each pixel in each bounding box gets mapped to a barycentric coordinate.
  // Outer list: triangles. Inner list: barycentric coordinates.
  // ***ALL BCOORDS IN BOX
  // ***
  std::vector<glm::vec3> bcoords;

  size_t number_of_pixels_for_this_triangle;
  // Now create a list containing only the bcoords inside the triangle.
  // &&&ALL BCOORDS IN TRIANGLE
  // ***
  std::vector<glm::vec3> triangle_bcoords;

  // Now create a list containing only the pixels inside the triangle.
  // &&&ALL PIXEL X, Y COORDS IN TRIANGLE
  // ***
  std::vector<glm::uvec2> triangle_pixels;

  // Now calculate the z coordinates for every pixel inside of every triangle.
  // Every pixel on the screen stores a z index.
  // Index every glm::ivec2 from above, update the Z coord and the color if needed.
  // When it comes time to render: For every entry in this array,
  // if the Z is greater than std::numeric_limits<float>::min(),
  // then take that glm::ivec2 and the color and update the pixel list.
  // &&&ALL PIXEL Z COORDS IN TRIANGLE
  // ***
  std::vector<float> z_values_per_triangle;

  // &&&ALL Z COORDS IN THE SCREEN ZBUFFER
  // Same size as the screen.
  RectangularArray<float> zbuffer;
  // &&&TRIANGLE/SHADE ARRAY INDEX ID FOR EACH FLOAT IN ZBUFFER
  // Same size as the screen.
  // Initialized to the Black Triangle (last element) in shades.
  RectangularArray<int> zbuffer_color_id;

  // Every pixel to be drawn to the screen goes here.
  // Same size as the screen.
  // Will use zbuffer_color_id and shades
  RectangularArray<Color> pixels;
};

void update_data(const glm::mat4& matrix, Data& data) {
  apply_matrix_transform(matrix, data.verts);
  expand_triangles(data.verts, data.faces, data.all_triangles);
  update_normals(matrix, data.all_normals);
  remove_invisible_triangles_and_normals(data.all_triangles, data.all_normals, data.width, data.height,
                                         data.visible_triangles_size, data.visible_triangles, data.visible_normals);
  calculate_light(data.visible_normals, data.visible_triangles_size, data.shades);
  data.zbuffer.set_all(std::numeric_limits<float>::min());
  // Set all of them to be the dummy triangle color (black).
  data.zbuffer_color_id.set_all(data.shades.size() - 1);

  calculate_bcaches(data.visible_triangles, data.visible_triangles_size, data.bcaches, data.temp1);

  calculate_bounding_boxes(data.visible_triangles, data.width, data.height, data.visible_triangles_size, data.boxes);

  // calculate_pixel_list_per_box(data.boxes, data.visible_triangles_size, data.pixel_list);

  // barycentric(data.bcaches, data.pixel_list, data.visible_triangles_size, data.bcoords);

  // bbox_pixel_and_bcoords_to_triangle(data.pixel_list, data.bcoords, data.visible_triangles_size, data.triangle_bcoords,
  //                                  data.triangle_pixels);
  // calculate_zvalues_per_pixel_per_triangle(data.visible_triangles, data.triangle_bcoords, data.visible_triangles_size,
  //                                         data.z_values_per_triangle);
  // update_zbuffer(data.triangle_pixels, data.z_values_per_triangle, data.visible_triangles_size, data.zbuffer,
  //               data.zbuffer_color_id);
  for (size_t i = 0; i != data.visible_triangles_size; ++i) {
    calculate_pixel_list_per_box(data.boxes[i], data.number_of_pixels_for_this_box, data.pixel_list);

    barycentric(data.bcaches[i], data.pixel_list, data.number_of_pixels_for_this_box, data.bcoords, data.temp1, data.temp2,
                data.temp3, data.temp4, data.temp5);
    bbox_pixel_and_bcoords_to_triangle(data.pixel_list, data.bcoords, data.number_of_pixels_for_this_box,
                                       data.number_of_pixels_for_this_triangle, data.triangle_bcoords, data.triangle_pixels);
    calculate_zvalues_per_pixel_per_triangle(data.visible_triangles[i], data.triangle_bcoords,
                                             data.number_of_pixels_for_this_triangle, data.z_values_per_triangle);
    update_zbuffer(i, data.triangle_pixels, data.z_values_per_triangle, data.number_of_pixels_for_this_triangle,
                   data.zbuffer, data.zbuffer_color_id);
  }
  update_pixels(data.shades, data.zbuffer_color_id, data.pixels);
}

void draw_pixels(RectangularArray<Color>& pixels, Frame& frame) {
  for (int y = 0; y != pixels.get_height(); ++y) {
    for (int x = 0; x != pixels.get_width(); ++x) {
      frame.put_pixel(x, y, pixels(x, y));
    }
  }
}

void init_data(std::string& file, int width, int height, const glm::mat4& matrix, Data& data) {
  data.width = width;
  data.height = height;

  data.temp1 = std::vector<float>(data.width * data.height);
  data.temp2 = std::vector<float>(data.width * data.height);
  data.temp3 = std::vector<float>(data.width * data.height);
  data.temp4 = std::vector<float>(data.width * data.height);
  data.temp5 = std::vector<float>(data.width * data.height);

  get_model(file, data.verts, data.faces);

  data.all_triangles = std::vector<Triangle>(data.faces.size());
  data.all_normals = std::vector<glm::vec3>(data.faces.size());

  expand_triangles(data.verts, data.faces, data.all_triangles);
  initialize_normals(data.all_triangles, data.all_normals);

  data.visible_triangles = std::vector<Triangle>(data.faces.size());
  data.visible_normals = std::vector<glm::vec3>(data.faces.size());

  // Has one extra elemnt storing a dummy triangle color (black).
  data.shades = std::vector<Color>(data.faces.size() + 1);
  data.bcaches = BarycentricCaches(data.faces.size());
  data.boxes = std::vector<Quad>(data.faces.size());

  // The bounding box could take up the entire screen, so make the buffers that big.
  data.pixel_list = std::vector<glm::uvec2>(data.width * data.height);
  data.bcoords = std::vector<glm::vec3>(data.width * data.height);
  data.triangle_bcoords = std::vector<glm::vec3>(data.width * data.height);
  data.triangle_pixels = std::vector<glm::uvec2>(data.width * data.height);
  data.z_values_per_triangle = std::vector<float>(data.width * data.height);

  data.zbuffer = RectangularArray<float>(width, height, std::numeric_limits<float>::min());
  data.zbuffer_color_id = RectangularArray<int>(width, height, 0);
  data.pixels = RectangularArray<Color>(width, height, Color{0, 0, 0});

  update_data(matrix, data);
}
