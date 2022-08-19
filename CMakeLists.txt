cmake_minimum_required(VERSION 3.8)

#oof
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c17 -fpermissive -Wall -Wformat-security -g")

project(gctools)

#our include dir
include_directories(include)
file(GLOB_RECURSE LIBRARY_SOURCE
    "include/*.h"
    "src/*.c"
)

add_library(gctools STATIC ${LIBRARY_SOURCE})

#add_executable(compressor examples/yay0compressor/main.c)
#add_executable(repack examples/tests/libtests.c)

#target_link_libraries(compressor gctools)
#target_link_libraries(repack gctools)
