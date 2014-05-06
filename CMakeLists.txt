project(Game)

cmake_minimum_required(VERSION 2.8)

#enable C++0x support for g++ and friends
if(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-std=c++0x")
endif(CMAKE_COMPILER_IS_GNUCXX)

#check for required libraries
find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_library(GLFW NAMES glfw glfw3 libglfw libglfw3)
if(NOT GLFW)
    message(FATAL_ERROR "GLFW 3 is required!")
endif(NOT GLFW)
find_library(ASSIMP NAMES assimp libassimp)
if(NOT ASSIMP)
    message(FATAL_ERROR "Assimp is required!")
endif(NOT ASSIMP)

#include our local directories
include_directories(include libpng-1.6.2 freetype-gl lz4-r112)
#because we're building GLEW as a static library ourselves
add_definitions(-DGLEW_STATIC)

add_subdirectory(src)
add_subdirectory(freetype-gl)
add_subdirectory(lz4-r112)
add_subdirectory(glew)

add_executable(game entrypoint.cpp)
target_link_libraries(game gamelib freetype-gl lz4 glew assimp pthread glfw ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES})
