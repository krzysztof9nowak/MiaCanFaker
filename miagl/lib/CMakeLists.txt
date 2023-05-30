cmake_minimum_required(VERSION 3.10)
project("miagl")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)
add_compile_options(-Wall -O2)

add_library(${PROJECT_NAME} STATIC
    src/miagl.c
    src/miagl-bitmap.c
    src/miagl-font.c
    src/miagl-gfx.c
)
target_compile_definitions(${PROJECT_NAME} PRIVATE MIAGL_IMPL)
target_include_directories(${PROJECT_NAME} PUBLIC inc)
