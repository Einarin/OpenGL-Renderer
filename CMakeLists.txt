project(Game)

cmake_minimum_required(VERSION 2.8)

#enable C++0x support for g++ and friends
if(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-std=c++0x")
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(include libpng-1.6.2 . lz4-r112)
add_subdirectory(src)
