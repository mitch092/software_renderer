cmake_minimum_required (VERSION 3.11)
project ("software_rasterizer")

# Include the git submodule source code files here.
add_subdirectory(extern/sdl2 
                 extern/glm)

add_executable(${PROJECT_NAME} src/main.cpp 
                               src/model.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE src/ 
                                           PRIVATE extern/sdl2/include 
										   PRIVATE extern/glm/glm)

target_link_libraries(${PROJECT_NAME} SDL2 SDL2main)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)


install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY assets DESTINATION bin)