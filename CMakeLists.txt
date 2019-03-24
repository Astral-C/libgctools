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

add_executable(gctools_test test/libtests.c)
target_link_libraries(gctools_test gctools)

add_custom_command(
  OUTPUT gctools_run_tests
  COMMAND gctools_test
  COMMENT "Running gctools Library Tests..."
  VERBATIM
)

add_custom_target(
  RunTests ALL
  DEPENDS gctools_run_tests
)