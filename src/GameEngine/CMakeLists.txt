project (GameEngine)

cmake_minimum_required(VERSION 2.8)

SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/local/boost_1_55_0")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/local/boost_1_55_0/lib")

set(NAME_INCLUDE "C:\\local" CACHE FILEPATH "Directory for includes.")
set(BOOST_DIRECTORY "C:\\local" CACHE FILEPATH "Directory for includes.")
set(THERON_INCLUDE "C:\\local" CACHE FILEPATH "Directory for includes.")

FIND_PACKAGE( Boost 1.55.0 )
if(Boost_FOUND)
	INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${NAME_INCLUDE} )
	ADD_EXECUTABLE( GameEngine GameEngine.cpp GameObject.cpp PC.cpp SpacialObject.cpp )
endif()
#TARGET_LINK_LIBRARIES( GameEngine.exe ${Boost_LIBRARIES}  )