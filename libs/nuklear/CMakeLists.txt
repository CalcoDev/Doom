## CUSTOM MADE BY CALCOPOD
## BAD LMAO
cmake_minimum_required(VERSION 3.20)
project(nuklear)

file(
    GLOB_RECURSE
    sources
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_library(nuklear STATIC "${sources}")
target_include_directories(nuklear PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# add_library(nuklear "include/glad/glad.h" "src/glad.c")
# target_include_directories(nuklear PUBLIC "include/")