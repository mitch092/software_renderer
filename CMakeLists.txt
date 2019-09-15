cmake_minimum_required (VERSION 3.11)
project ("software_rasterizer")
include(FetchContent)

FetchContent_Declare(
  sdl2
  GIT_REPOSITORY https://github.com/spurious/SDL-mirror
  GIT_TAG origin/release-2.0.10
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm
  GIT_TAG origin/0.9.8
)

FetchContent_MakeAvailable(sdl2 glm)



add_executable(${PROJECT_NAME} src/main.cpp)




#target_include_directories(${PROJECT_NAME} PRIVATE src/ 
#                                           PRIVATE "${SDL2_INCLUDE_DIR}" 
#	                                       PRIVATE "${GLM_INCLUDE_DIR}")


target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static SDL2main glm)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
target_compile_options(${PROJECT_NAME} PUBLIC "/W3" "/DEBUG:FULL>")
target_link_options(${PROJECT_NAME} PUBLIC "/MTd")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
target_compile_options(${PROJECT_NAME} PUBLIC "/W0" "/debug:full" "/fast")
endif()


file(COPY assets/african_head.obj DESTINATION ${CMAKE_BINARY_DIR})