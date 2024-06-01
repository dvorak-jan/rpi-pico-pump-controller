cmake_minimum_required(VERSION 3.12)

#include(pico_sdk_import.cmake)
# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()

include(pico_sdk_import.cmake)

add_subdirectory(pico-cpp)

project(pump_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)


pico_sdk_init()

add_executable(pump_project
    src/main.cpp
)

# enable usb output, disable uart output
pico_enable_stdio_usb(pump_project 1)
pico_enable_stdio_uart(pump_project 0)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(pump_project pico_stdlib hardware_adc)

add_library(FreeRTOS STATIC 
FreeRTOS-Kernel/croutine.c
FreeRTOS-Kernel/event_groups.c
FreeRTOS-Kernel/list.c
FreeRTOS-Kernel/queue.c
FreeRTOS-Kernel/stream_buffer.c
FreeRTOS-Kernel/tasks.c
FreeRTOS-Kernel/timers.c 
FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c 
FreeRTOS-Kernel/portable/MemMang/heap_4.c 
src/FreeRTOS_Support_Files/IdleMemory.c)

target_include_directories(FreeRTOS PUBLIC FreeRTOS-Kernel/include include/ FreeRTOS-Kernel/portable/GCC/ARM_CM0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(pump_project)
target_link_libraries(pump_project FreeRTOS pico-cpp)
add_custom_command(TARGET pump_project
        POST_BUILD 
        COMMAND arm-none-eabi-size pump_project.elf
        )
# rest of your project

