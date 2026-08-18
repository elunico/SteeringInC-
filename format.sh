#!/bin/zsh

clang-format -i src/*.cpp
clang-format -i src/ui/*.cpp
clang-format -i include/ui/*.h
clang-format -i include/*.h