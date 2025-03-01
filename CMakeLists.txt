cmake_minimum_required (VERSION 3.28)
project (software_rasterizer VERSION 0.1 LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(
  sdl2
  GIT_REPOSITORY https://github.com/spurious/SDL-mirror
  GIT_TAG origin/release-2.0.10
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm
  GIT_TAG origin/master
)
FetchContent_MakeAvailable(sdl2 glm)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static SDL2main glm)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

file(COPY assets/african_head.obj DESTINATION ${CMAKE_BINARY_DIR})