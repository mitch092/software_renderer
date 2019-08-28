#pragma once
#include <fstream>
#include <optional>
#include <string>

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
void get_model(const std::ifstream& file, std::vector<glm::vec3>& verts, std::vector<glm::ivec3>& faces) {
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

