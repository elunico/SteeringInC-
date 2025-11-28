cmake_minimum_required(VERSION 3.14)

project(vehicles)

include_directories(.)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -O0 -g -fsanitize=address -fsanitize=undefined")


add_executable(main vec2d.cpp vehicle.cpp world.cpp dna.cpp utils.cpp main.cpp)