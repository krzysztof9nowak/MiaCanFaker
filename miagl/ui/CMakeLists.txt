cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME "miaui")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)
add_compile_options(-Wall -O2)

add_library(${PROJECT_NAME} STATIC
    src/dummy.c
)
target_include_directories(${PROJECT_NAME} PUBLIC inc/)
