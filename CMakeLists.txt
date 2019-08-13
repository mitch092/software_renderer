cmake_minimum_required (VERSION 3.11)
project ("software_rasterizer")

# Include the source code for this project.
include_directories("${PROJECT_SOURCE_DIR}/src/")
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Include the git submodule source code files here.
add_subdirectory("${PROJECT_SOURCE_DIR}/extern/sdl2")
include_directories("${PROJECT_SOURCE_DIR}/extern/sdl2/include")

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_link_libraries(${PROJECT_NAME} SDL2 SDL2main)