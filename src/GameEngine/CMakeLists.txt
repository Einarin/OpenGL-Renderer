cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE( Boost 1.5 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

ADD_EXECUTABLE( GameEngine.exe GameEngine.cpp GameObject.cpp PC.cpp )

TARGET_LINK_LIBRARIES( GameEngine.exe ${Boost_LIBRARIES}  )