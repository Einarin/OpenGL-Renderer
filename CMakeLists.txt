project(Game)

cmake_minimum_required(VERSION 2.8)

#enable C++0x support for g++ and friends
if(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-std=c++0x")
endif(CMAKE_COMPILER_IS_GNUCXX)

find_package(Freetype REQUIRED)
#find_package(ZLIB REQUIRED)
#link_directories(libpng)
#find_library(PNG16 NAMES png16 libpng16)
#find_library(GLEW NAMES glew libglew glew32 libglew32)

include_directories(include libpng-1.6.2 freetype-gl lz4-r112)

add_definitions(-DGLEW_STATIC)

add_subdirectory(src)
add_subdirectory(freetype-gl)
add_subdirectory(lz4-r112)
add_subdirectory(glew)

#add_subdirectory(libpng-1.6.2)

#add_subdirectory(glfw-3.0.4)
#add_subdirectory(assimp--3.0.1270-source-only)

add_executable(game entrypoint.cpp)
target_link_libraries(game gamelib freetype-gl lz4 glew png16 assimp pthread glfw ${FREETYPE_LIBRARIES})
