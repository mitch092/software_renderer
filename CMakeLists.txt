cmake_minimum_required (VERSION 3.11)
project ("software_rasterizer")

# In order to fix a weird linker error ("SDL_string.obj : error LNK2019: unresolved external symbol memset referenced in function SDL_vsnprintf_REAL"),
# put -DLIBC=YES on the commandline.

# Include the git submodule source code files here.
add_subdirectory(extern/sdl2 
                 extern/glm)

add_executable(${PROJECT_NAME} src/main.cpp 
                               src/model.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE src/ 
                                           PRIVATE extern/sdl2/include 
	                                       PRIVATE extern/glm/glm)



target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static SDL2main)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:/Wall>")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:/GL /GS->")
target_link_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:/LTCG>")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:/fast>")
#target_link_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:   >")
endif()


file(COPY assets/african_head.obj DESTINATION ${CMAKE_BINARY_DIR})