#pragma once
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "Primitives.h"

std::optional<std::string> file_to_string(const char* filename) {
  std::ifstream in(filename, std::ifstream::in);
  if (!in.fail()) {
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    std::string buffer(size, ' ');
    in.seekg(0);
    in.read(&buffer[0], size);
    return {buffer};
  } else {
    return std::nullopt;
  }
}

void parse_object_file_string(std::string& _file, std::vector<glm::vec3>& verts, std::vector<glm::uvec3>& faces) {
  // I have to move the data in the file around as a std::string and convert it to a stream here,
  // because std::optional<std::ifstream> does not work for me!
  std::istringstream file(_file);

  verts.clear();
  faces.clear();
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
}

struct Mesh {
  std::vector<glm::uvec3> faces;
  std::vector<glm::vec3> vertices; 
};

std::optional<Mesh> get_mesh(const char* filename) {
  auto fileString = file_to_string(filename);
  if (!fileString) return std::nullopt;
  Mesh mesh;
  parse_object_file_string(fileString, mesh.vertices, mesh.faces);
  return mesh;
}
