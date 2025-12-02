cmake_minimum_required(VERSION 3.14)

project(vehicles)

include_directories(.)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O0 -g -fsanitize=address -fsanitize=undefined")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic  -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEFLT_NO_QT")

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

find_package(FLTK REQUIRED)
include_directories(${FLTK_INCLUDE_DIR})
add_executable(main vec2d.cpp vehicle.cpp world.cpp dna.cpp fltkrenderer.cpp food.cpp utils.cpp main.cpp
        cursesrenderer.cpp  controls.cpp)
target_link_libraries(main ${CURSES_LIBRARIES})
target_link_libraries(main ${FLTK_LIBRARIES} ${FLTK_EXTRA_LIBRARIES})
