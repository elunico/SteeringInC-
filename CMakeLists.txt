cmake_minimum_required(VERSION 3.14)

project(vehicles)

include_directories(.)
include_directories(include)

option(RELEASE_BUILD "Build in release mode" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

if(RELEASE_BUILD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -Werror")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer -D_GLIBCXX_DEBUG")
    if(ADDRESS_SANITIZER)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fsanitize=address")
    endif()
    if(THREAD_SANITIZER)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fsanitize=thread")
    endif()
endif()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DNO_TPS_LIMIT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DFAST_RANDOM") # Use faster but less random number generator

find_package(FLTK REQUIRED)
include_directories(${FLTK_INCLUDE_DIR})
add_executable(main ${SOURCES})
target_link_libraries(main ${FLTK_LIBRARIES} ${FLTK_EXTRA_LIBRARIES})
