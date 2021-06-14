cmake_minimum_required(VERSION 3.8)

#oof
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fpermissive -Wall -Wformat-security")

project(gctools)

#our include dir
include_directories(include)
file(GLOB_RECURSE LIBRARY_SOURCE
    "include/*.h"
    "src/*.c"
)

add_library(gctools STATIC ${LIBRARY_SOURCE})