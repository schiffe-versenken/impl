cmake_minimum_required(VERSION 3.13)
project(test)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(test
        defs.cpp
        defs.h
        main.cpp
        ship_generator.h
        strategies.cpp
        strategies.h
        utils.cpp
        utils.h)

find_library(PTHREAD pthread)
target_link_libraries(test "${PTHREAD}")
find_library(GMP gmp)
target_link_libraries(test "${GMP}")