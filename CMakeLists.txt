cmake_minimum_required(VERSION 3.20)
project(nioa_pso)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(nioa_pso
        src/main.cpp src/mkp.h src/DataLoader.cpp src/DataLoader.h)
