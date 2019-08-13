cmake_minimum_required (VERSION 3.11)
project ("glfwTest")

# Include the source code for this project.
include_directories("${PROJECT_SOURCE_DIR}/src/")
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Include the git submodule source code files here.
add_subdirectory("${PROJECT_SOURCE_DIR}/extern/glm")
include_directories("${PROJECT_SOURCE_DIR}/extern/glm/glm")

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_link_libraries(${PROJECT_NAME} glm ${GLM_LIBRARIES})