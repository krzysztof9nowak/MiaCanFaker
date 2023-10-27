cmake_minimum_required(VERSION 3.10)

# Include toolchain file
include(stm32-arm-toolchain.cmake)

# Configure the project
project(ecu_firmware C ASM)
set(CMAKE_C_STANDARD 11)

# Include miagl lib
add_subdirectory(../miagl ${PROJECT_BINARY_DIR}/miagl)

set(STARTUP_ASM "startup_stm32f103xe.s")

file(GLOB_RECURSE SOURCES 
    "Core/Src/*.*" 
    "Drivers/STM32F1xx_HAL_Driver/*.*" 
    "Middlewares/Third_Party/FreeRTOS/Source/croutine.c"
    "Middlewares/Third_Party/FreeRTOS/Source/event_groups.c"
    "Middlewares/Third_Party/FreeRTOS/Source/list.c"
    "Middlewares/Third_Party/FreeRTOS/Source/queue.c"
    "Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"
    "Middlewares/Third_Party/FreeRTOS/Source/tasks.c"
    "Middlewares/Third_Party/FreeRTOS/Source/timers.c"
    "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c"
    "Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c"
    "Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c"
)

include_directories(
    Core/Inc
    Core/Inc/tasks
    Drivers/STM32F1xx_HAL_Driver/Inc
    Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3
    Drivers/CMSIS/Device/ST/STM32F1xx/Include
    Drivers/CMSIS/Include
)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${STARTUP_ASM} ${LINKER_SCRIPT})
target_link_libraries(${PROJECT_NAME}.elf m)
target_link_libraries(${PROJECT_NAME}.elf miaui)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")

# Custom targets for interfacing with openocd

add_custom_target(flash 
        COMMAND openocd -f openocd.cfg -c 'init' -c 'targets' -c 'reset init' -c 'flash write_image erase ${HEX_FILE}' -c 'reset run' -c shutdown
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Building and flashing ${PROJECT_NAME}.elf using openocd"
        SOURCES openocd.cfg
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(flashonly
        COMMAND openocd -f openocd.cfg -c 'init' -c 'targets' -c 'reset init' -c 'flash write_image erase ${HEX_FILE}' -c 'reset run' -c shutdown
        COMMENT "Flashing ${PROJECT_NAME}.elf using openocd"
        SOURCES openocd.cfg
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(flashesp
        COMMAND arm-none-eabi-gdb -ex "target extended-remote 192.168.4.1:2022" -ex "set mem inaccessible-by-default off" -ex "load ${HEX_FILE}" -ex "monitor reset halt" -ex "detach" -ex "quit"
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Running gdb"
)

add_custom_target(debug
        COMMAND openocd -f openocd.cfg -c 'init' -c 'targets' -c 'reset halt'
        COMMENT "Running openocd debug"
        SOURCES openocd.cfg
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(database ALL
        COMMAND sh make_database.sh
        DEPENDS ${PROJECT_NAME}.elf
        BYPRODUCTS ${CMAKE_SOURCE_DIR}/compile_commands.json
        COMMENT "Generating compile_commands.json"
        SOURCES make_database.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

