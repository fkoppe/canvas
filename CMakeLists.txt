cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(cnvx LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(canvas)

target_link_libraries(
    canvas
    PRIVATE
    spore
)

add_subdirectory(include)
add_subdirectory(library)
add_subdirectory(source)