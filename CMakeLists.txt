cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(TARGET ldjam37)
project(${TARGET})

# includes
#set(SFML_ROOT "C:/Program Files (x86)/SFML")
find_package(SFML COMPONENTS audio graphics window system)
include_directories(${SFML_INCLUDE_DIR})
#include_directories("E:/dev/usr/include")

# libraries
set(LIBS
	${SFML_LIBRARIES}
	${SFML_DEPENDENCIES}
)

# source files
set(SRC
	src/common.hpp
	src/asset.hpp
	src/persist.hpp
	
	src/timing.hpp
	src/input.hpp
	src/scene.hpp
	src/room.hpp
	src/actor.hpp
	src/prop.hpp
	src/torchlight.hpp
	src/main.cpp
)

add_executable(${TARGET} ${SRC})
target_link_libraries(${TARGET} ${LIBS})
