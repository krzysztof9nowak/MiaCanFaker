cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME "x86-miagl-tests")

project(${PROJECT_NAME})

find_package(SDL2 REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)
add_compile_options(-Wall -O2)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(ui)

add_executable(${PROJECT_NAME} x86-main.c)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} miaui)
