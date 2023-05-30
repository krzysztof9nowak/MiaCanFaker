cmake_minimum_required(VERSION 3.10)
project("miaui")
set(X86_NAME "x86-miagl-tests")

option(NO_X86_TARGET "Explicitly disable x86 target" OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)
add_compile_options(-Wall -O2)

add_subdirectory(lib)

set(C_SOURCES
    "src/miaui-resources.c"
    "src/miaui-test.c"
)

if ((CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64") AND NOT NO_X86_TARGET)
    # x86 target for development without MCU

    find_package(SDL2 REQUIRED)

    add_executable(${X86_NAME} x86-main.c ${C_SOURCES})
    target_include_directories(${X86_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_include_directories(${X86_NAME} PRIVATE inc)
    target_link_libraries(${X86_NAME} ${SDL2_LIBRARIES})
    target_link_libraries(${X86_NAME} miagl)
endif()

add_library(${PROJECT_NAME} STATIC ${C_SOURCES})
target_link_libraries(${PROJECT_NAME} miagl)
target_include_directories(${PROJECT_NAME} PUBLIC inc)

