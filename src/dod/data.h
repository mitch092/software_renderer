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

  // Get this from model.h
  // Transform matrices are applied here.
  std::vector<glm::vec3> verts;
  std::vector<glm::ivec3> faces;

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

  // Lighting for each triangle.
  // Resize this to be the same size as triangles.
  std::vector<Color> shades;

  // Calculate whatever I can for barycentric coords using only the Triangles and store it here.
  // Resize this to be the same size as triangles.
  std::vector<BarycentricCache> bcaches;

  // Bounding box data. Every triangle gets a bounding box.
  // Don't need to take care of invisible triangles, as the check is done during the filtering of visible_triangles.
  std::vector<Quad> boxes;
  // And every bounding box gets a list of pixels.
  // Outer list: triangles. Inner list: pixel coordinates (x, y).
  // ***ALL PIXELS IN BOX FOR EVERY BOX
  JaggedArray<glm::ivec2> pixel_list;

  // Each pixel in each bounding box gets mapped to a barycentric coordinate.
  // Outer list: triangles. Inner list: barycentric coordinates.
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
void update(const glm::mat4& matrix, Data& data) {
  apply_matrix_transform(matrix, data.verts);
  expand_triangles(data.verts, data.faces, data.all_triangles);
  update_normals(matrix, data.all_normals);
  remove_invisible_triangles_and_normals(data.all_triangles, data.all_normals, data.width, data.height,
                                         data.visible_triangles, data.visible_normals);
  calculate_light(data.visible_normals, data.shades);
  data.zbuffer.set_all(std::numeric_limits<float>::min());
  data.zbuffer_color_id.set_all(data.shades.size() - 1);
  calculate_bcaches(data.visible_triangles, data.bcaches);
  calculate_bounding_boxes(data.visible_triangles, data.width, data.height, data.boxes);
  calculate_pixel_list_per_box(data.boxes, data.pixel_list);
  barycentric(data.bcaches, data.pixel_list, data.bcoords);
  bbox_pixel_and_bcoords_to_triangle(data.pixel_list, data.bcoords, data.triangle_bcoords, data.triangle_pixels);
  calculate_zvalues_per_pixel_per_triangle(data.visible_triangles, data.triangle_bcoords, data.z_values_per_triangle);
  update_zbuffer(data.triangle_pixels, data.z_values_per_triangle, data.zbuffer, data.zbuffer_color_id);
  update_pixels(data.shades, data.zbuffer_color_id, data.pixels);
}

void draw_pixels(RectangularArray<Color>& pixels, Frame& frame) {
  for (int y = 0; y != pixels.get_height(); ++y) {
    for (int x = 0; x != pixels.get_width(); ++x) {
      frame.put_pixel(x, y, pixels(x, y));
    }
  }
}

void init_data(std::ifstream& file, int width, int height, Data& data) {
  data.width = width;
  data.height = height;

  get_model(file, data.verts, data.faces);

  data.all_triangles.reserve(data.faces.size());
  data.all_normals.reserve(data.faces.size());

  expand_triangles(data.verts, data.faces, data.all_triangles);
  initialize_normals(data.all_triangles, data.all_normals);

  data.visible_triangles.reserve(data.faces.size());
  data.visible_normals.reserve(data.faces.size());

  data.shades.reserve(data.faces.size());
  data.bcaches.reserve(data.faces.size());
  data.boxes.reserve(data.faces.size());
  data.pixel_list.reserve(data.faces.size());
  data.bcoords.reserve(data.faces.size());
  data.triangle_bcoords.reserve(data.faces.size());
  data.triangle_pixels.reserve(data.faces.size());
  data.z_values_per_triangle.reserve(data.faces.size());

  data.zbuffer = RectangularArray<float>(width, height, std::numeric_limits<float>::min());
  data.zbuffer_color_id = RectangularArray<int>(width, height, 0);
  data.pixels = RectangularArray<Color>(width, height, Color{255, 255, 255});

  update(center_and_scale(width, height), data);
}
