/*#include <iostream>

#include "App.h"
#include "dod/model.h"

int main(int argc, char* argv[]) {
  std::string str = get_model_file_data("african_head.obj");

  auto app = App("tinyrenderer in SDL", 800, 800, str);
  app.display();

  return EXIT_SUCCESS;
}
*/

#include <SDL.h>

#include <cstdlib>
#include <fstream>
#include <glm/ext/quaternion_float.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#include "Benchmark.h"
#include "Color.h"
#include "Primitives.h"
#include "dod/RectangularArray.h"
#include "dod/barycentric.h"
#include "dod/box.h"
#include "dod/lighting.h"

struct WindowInfo {
  std::string name;
  Uint32 width;
  Uint32 height;
};

struct Transform {
  glm::vec3 position;
  glm::quat orientation;
  glm::vec3 scale;
};

struct Mesh {
  const std::vector<glm::vec3> original_vertex_array;
  std::vector<glm::vec3> transformed_vertex_array;
  const std::vector<glm::uvec3> index_array;
};

struct Scene {
  // The meshes to load into this scene.
  std::vector<std::string> mesh_names;
  // Two or more models can point to the same mesh (instancing).
  std::vector<int> model_to_mesh_names_id;
  std::vector<Transform> model_transforms;
  // A camera is just a transform. Can have multiple in a scene, but only one active camera.
  std::vector<Transform> camera_transforms;
  Uint32 active_camera_id;
  // Directional lights are just normalized 3D directional vectors.
  std::vector<glm::vec3> directional_lights;
  // Point lights are 3D point vectors (not implemented).
};

std::string read_file(std::string filename) {
  std::ifstream in(filename, std::ifstream::in);
  assert(!in.fail());
  in.seekg(0, std::ios::end);
  size_t size = in.tellg();
  std::string buffer(size, ' ');
  in.seekg(0);
  in.read(&buffer[0], size);
  return buffer;
}

Mesh get_mesh(std::string filename) {
  std::string _file = read_file(filename);
  std::istringstream file(_file);

  std::vector<glm::vec3> verts;
  std::vector<glm::uvec3> faces;

  std::string line;
  while (!file.eof()) {
    std::getline(file, line);
    std::istringstream iss(line);
    char trash;
    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      glm::vec3 v;
      iss >> v.x;
      iss >> v.y;
      iss >> v.z;
      verts.push_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      std::vector<unsigned int> f;
      int itrash, idx;
      iss >> trash;
      while (iss >> idx >> trash >> itrash >> trash >> itrash) {
        idx--;  // in wavefront obj all indices start at 1, not zero
        f.push_back(idx);
      }
      faces.emplace_back(f[0], f[1], f[2]);
    }
  }
  std::cerr << "# v# " << verts.size() << " f# " << faces.size() << std::endl;
  return Mesh{verts, verts, faces};
}

std::vector<Mesh> get_meshes(std::vector<std::string> filenames) {
  std::vector<Mesh> meshes;
  for (std::string name : filenames) {
    meshes.push_back(get_mesh(name));
  }
  return meshes;
}

// Checking if a triangle is in the frame is pretty verbose if I expand it out by hand,
// So I'm keeping it factored out of expand_triangles() and using inlines to hopefully
// get the same result as expanding it out by hand.
inline bool pixel_in_frame(int x, int y, int width, int height) { return (0 <= x && x < width) && (0 <= y && y < height); }

// If any of the 2D vertices of the triangle are in the frame, then keep the triangle.
inline bool triangle_in_frame(const Triangle& tri, int width, int height) {
  return pixel_in_frame(tri.a.x, tri.a.y, width, height) || pixel_in_frame(tri.b.x, tri.b.y, width, height) ||
         pixel_in_frame(tri.c.x, tri.c.y, width, height);
}

inline glm::mat4 getTransformMatrix(Transform& transform) {
  return glm::translate(transform.position) * glm::toMat4(transform.orientation) * glm::scale(transform.scale);
}

inline glm::mat4 getInverseTransformMatrix(Transform& transform) {
  return glm::translate(-1.0f * transform.position) * glm::toMat4(glm::conjugate(transform.orientation)) *
         glm::scale(glm::mat4(1.0f),
                    glm::vec3(1.0f / transform.scale.x, 1.0f / transform.scale.y, 1.0f / transform.scale.z));
}

int main(int argc, char* argv[]) {
  WindowInfo windowInfo{"tinyrenderer in SDL", 800, 800};

  if (int init = SDL_Init(SDL_INIT_VIDEO); init >= 0) {
    if (SDL_Window* window = SDL_CreateWindow(windowInfo.name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                              windowInfo.width, windowInfo.height, SDL_WINDOW_SHOWN);
        window != nullptr) {
      if (SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); renderer != nullptr) {
        if (SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                                     windowInfo.width, windowInfo.height);
            texture != nullptr) {
          Scene scene;
          scene.mesh_names.push_back("african_head.obj");

          scene.model_to_mesh_names_id.push_back(0);
          scene.model_transforms.push_back(
              Transform{glm::vec3(0, 0, 0), glm::quatLookAt(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)), glm::vec3(1, 1, 1)});

          scene.camera_transforms.push_back(
              Transform{glm::vec3(0, 0, 3), glm::quatLookAt(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)), glm::vec3(1, 1, 1)});
          scene.active_camera_id = 0;

          scene.directional_lights.push_back(glm::vec3(0, 0, 1));

          std::vector<Mesh> meshes = get_meshes(scene.mesh_names);

          RectangularArray<Color> frame(windowInfo.width, windowInfo.height, Color());
          RectangularArray<float> depthbuffer(windowInfo.width, windowInfo.height, std::numeric_limits<float>::min());

          glm::mat4 proj =
              glm::perspective(glm::radians(45.0f), (float)windowInfo.width / (float)windowInfo.height, 0.1f, 100.0f);

          Stopwatch watch;
          bool quit = false;
          SDL_Event e;

          while (quit == false) {
            watch.update();

            while (SDL_PollEvent(&e) != 0) {
              if (e.type == SDL_QUIT) {
                quit = true;
              }
            }

            scene.model_transforms[0].orientation = glm::rotate(
                scene.model_transforms[0].orientation, watch.get_deltatime() * glm::two_pi<float>(), glm::vec3(0, 1, 0));

            glm::mat4 view = getInverseTransformMatrix(scene.camera_transforms[scene.active_camera_id]);

            for (int i = 0; i != scene.model_transforms.size(); ++i) {
              glm::mat4 model = getTransformMatrix(scene.model_transforms[i]);
              Mesh& mesh = meshes[scene.model_to_mesh_names_id[i]];
              for (int j = 0; j != mesh.original_vertex_array.size(); ++j) {
                //mesh.transformed_vertex_array[j] = glm::project(mesh.original_vertex_array[j], view * model, proj,
                //                                                glm::vec4{0.0f, 0.0f, windowInfo.width, windowInfo.height});
                glm::vec4 vec4(proj * view * model * glm::vec4(mesh.original_vertex_array[j], 1.0f));
                vec4 /= vec4.w;
                glm::vec3 vec3(vec4);
                vec3 += 1.0f;
                vec3 *= 0.5f;

                vec3.x *= windowInfo.width;
                vec3.y *= windowInfo.height;
                vec3.z *= 10000; //??? 

                mesh.transformed_vertex_array[j] = vec3;
              }
            }

            frame.set_all(Color());
            depthbuffer.set_all(std::numeric_limits<float>::min());

            for (int i = 0; i != scene.model_transforms.size(); ++i) {
              Mesh& mesh = meshes[scene.model_to_mesh_names_id[i]];
              for (int j = 0; j != mesh.index_array.size(); ++j) {
                // Expand a triangle.
                const glm::uvec3& face = mesh.index_array[j];
                Triangle tri{mesh.transformed_vertex_array[face.x], mesh.transformed_vertex_array[face.y],
                             mesh.transformed_vertex_array[face.z]};
                // Check if the triangle is in the frame.
                if (triangle_in_frame(tri, frame.get_width(), frame.get_height())) {
                  // Get its normal vector.

                  glm::vec3 normal = glm::normalize(glm::cross(tri.c - tri.a, tri.b - tri.a));
                  // Calculate the shade of the triangle (the color of all of the pixels).
                  // float intensity = glm::clamp(glm::dot(scene.directional_lights[0], normal), 0.0f, 1.0f);
                  float intensity = calculate_directional_light(scene.directional_lights, normal);

                  Uint8 shade = static_cast<Uint8>(intensity * 255);
                  Color triangleShade{shade, shade, shade};
                  // Create a barycentric cache.
                  BarycentricCache bary{tri};
                  glm::uvec4 bbox = calculate_bounding_box(tri, frame.get_width(), frame.get_height());

                  // left bottom right top
                  // x    y      z     w

                  // For every pixel in the bounding box,
                  for (auto y = bbox.y; y != bbox.w + 1; ++y) {
                    for (auto x = bbox.x; x != bbox.z + 1; ++x) {
                      // then calculate the barycentric coordinates.
                      glm::vec3 bcoords = bary.calculate_barycentric_coordinates(glm::uvec2{x, y});
                      // If the pixel is inside the triangle,
                      if (bcoords.x >= 0.0f && bcoords.y >= 0.0f && bcoords.z >= 0.0f) {
                        // then calculate the z value for the pixel.
                        float z = tri.a.z * bcoords.x + tri.b.z * bcoords.y + tri.c.z * bcoords.z;
                        // If the z value is greater than what's currencly in the depth buffer,
                        decltype(auto) depthbuffer_z = depthbuffer.get(x, y);
                        if (depthbuffer_z.has_value() && z > depthbuffer_z.value()) {
                          // then update the depth buffer and the framebuffer.
                          depthbuffer.set(x, y, z);
                          frame.set(x, y, triangleShade);
                        }
                      }
                    }
                  }
                }
              }
            }

            SDL_UpdateTexture(texture, nullptr, frame.data(), frame.get_width() * sizeof(Color));
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);

            std::cerr << watch.get_fps_as_string();
          }

          SDL_DestroyTexture(texture);
        }
        SDL_DestroyRenderer(renderer);
      }
      SDL_DestroyWindow(window);
    }
    SDL_Quit();
  }
  return EXIT_SUCCESS;
}