# Find gcc compiler
execute_process(COMMAND which arm-none-eabi-gcc OUTPUT_VARIABLE ARMGCC_PATH)
string(STRIP "${ARMGCC_PATH}" ARMGCC_PATH)
if (NOT EXISTS "${ARMGCC_PATH}")
    message( FATAL_ERROR "Could not find arm-none-eabi-gcc toolchain. Exiting." )
endif()

# Specify the cross compiler
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_C_COMPILER ${ARMGCC_PATH})
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(AS arm-none-eabi-as)
set(AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F103VETx_FLASH.ld)

set(COMMON_FLAGS "-mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections -specs=nosys.specs -specs=nano.specs -lnosys")

set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--no-warn-rwx-segment -T ${LINKER_SCRIPT}")

add_definitions(
    -D__weak=__attribute__\(\(weak\)\) 
    -D__packed=__attribute__\(\(__packed__\)\) 
    -DUSE_HAL_DRIVER 
    -DSTM32F103xE
)
