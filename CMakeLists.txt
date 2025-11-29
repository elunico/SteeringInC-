cmake_minimum_required(VERSION 3.14)

project(vehicles)

include_directories(.)

set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -O0 -g -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -O3")

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

find_package(FLTK REQUIRED)
include_directories(${FLTK_INCLUDE_DIR})
add_executable(main vec2d.cpp vehicle.cpp world.cpp dna.cpp fltkrenderer.cpp food.cpp utils.cpp main.cpp
        cursesrenderer.cpp
        cursesrenderer.h)
target_link_libraries(main ${CURSES_LIBRARIES})
target_link_libraries(main ${FLTK_LIBRARIES} ${FLTK_EXTRA_LIBRARIES})
