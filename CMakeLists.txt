cmake_minimum_required (VERSION 3.28)
project (software_rasterizer VERSION 0.1 LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(
  sdl2
  URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.30.12.tar.gz
  URL_HASH SHA256=560da2e54dd8af933e35bd08fb1b6cf80d4f6938c67710fecf13b7e9bdd6c47e
  DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/external/downloads
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/sdl2
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_Declare(
  glm
  URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.tar.gz
  URL_HASH SHA256=9f3174561fd26904b23f0db5e560971cbf9b3cbda0b280f04d5c379d03bf234c
  DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/external/downloads
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/glm
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(sdl2 glm)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static SDL2main glm)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

file(COPY assets/african_head.obj DESTINATION ${CMAKE_BINARY_DIR})