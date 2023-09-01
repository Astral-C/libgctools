cmake_minimum_required(VERSION 3.8)

project(gctools)

#our include dir
include_directories(include)
file(GLOB_RECURSE LIBRARY_SOURCE
    "include/*.h"
    "src/*.c"
)

add_library(gctools STATIC ${LIBRARY_SOURCE})
set_property(TARGET gctools PROPERTY C_STANDARD 17)
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
# /DEBUG is automatically set when in debug config
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /permissive- /Wall")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpermissive -Wall -Wformat-security -g")
endif()

#add_executable(compressor examples/yay0compressor/main.c)
#add_executable(repack examples/tests/libtests.c)

#target_link_libraries(compressor gctools)
#target_link_libraries(repack gctools)
