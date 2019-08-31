#pragma once
#include <fstream>
#include <optional>
#include <string>
#include <vector>

// Make sure the file can be opened.
// Keep this separated from the act of reading the file.
// Isolating the parts of the program that can fail seems like a good idea to me.
std::optional<std::ifstream> get_model_file_descriptor(const char* filename) {
  std::ifstream in(filename, std::ifstream::in);
  if (!in.fail()) {
    return {in};
  } else {
    return std::nullopt;
  }
}

// Now that we have a valid file handle, write it into buffers just like everything else.
void get_model(std::ifstream& file, std::vector<glm::vec3>& verts, std::vector<glm::ivec3>& faces) {
  verts.clear();
  faces.clear();
  std::string line;
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
      verts.push_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      std::vector<int> f;
      int itrash, idx;
      iss >> trash;
      while (iss >> idx >> trash >> itrash >> trash >> itrash) {
        idx--;  // in wavefront obj all indices start at 1, not zero
        f.push_back(idx);
      }
      faces.emplace_back(f[0], f[1], f[2]);
    }
  }
  std::cerr << "# v# " << model.verts.size() << " f# " << model.faces.size() << std::endl;
}

void apply_matrix_transform(const glm::mat4& matrix, std::vector<glm::vec3>& verts) {
  for (int i = 0; i != verts.size(); ++i) {
    auto vert4 = matrix * glm::vec4(verts[i], 1.0f);
    auto vert3 = glm::vec3(vert4);
    verts[i] = vert3;
  }
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

// Called once every frame. Finds every triangle.
void expand_triangles(const std::vector<glm::vec3>& vertices, const std::vector<glm::ivec3>& faces,
                      std::vector<Triangle>& triangles) {
  triangles.clear();
  for (int i = 0; i != faces.size(); ++i) {
    triangles.emplace_back(vertices[faces[i].x], vertices[faces[i].y], vertices[faces[i].z]);
  }
}

// Called once. Finds every normal for every triangle.
void initialize_normals(const std::vector<Triangle>& triangles, std::vector<glm::vec3>& normals) {
  normals.clear();
  for (int i = 0; i != triangles.size(); ++i) {
    const auto& tri = triangles[i];
    normals.push_back(glm::normalize(glm::cross(tri.c - tri.a, tri.b - tri.a)));
  }
}

// Called every frame, after initialize_normals figures out the normals for the first time.
void update_normals(const glm::mat4& matrix, std::vector<glm::vec3>& normals) {
  const glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(matrix));
  for (int i = 0; i != normals.size(); ++i) {
    normals[i] = normalMatrix * normals[i];
  }
}

void remove_invisible_triangles_and_normals(const std::vector<Triangle>& all_triangles,
                                            const std::vector<glm::vec3>& all_normals, const int width, const int height,
                                            std::vector<Triangle>& visible_triangles,
                                            std::vector<glm::vec3>& visible_normals) {
  visible_triangles.clear();
  visible_normals.clear();
  for (int i = 0; i != all_triangles.size(); ++i) {
    if (triangle_in_frame(all_triangles[i], width, height)) {
      visible_triangles.push_back(all_triangles[i]);
      visible_normals.push_back(all_normals[i]);
    }
  }
}