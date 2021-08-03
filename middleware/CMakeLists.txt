cmake_minimum_required(VERSION 3.16)
project(target-middleware LANGUAGES C CXX)

add_library(middleware STATIC
    FreeRTOSv202012.00/FreeRTOS/Source/croutine.c
    FreeRTOSv202012.00/FreeRTOS/Source/event_groups.c
    FreeRTOSv202012.00/FreeRTOS/Source/list.c
    FreeRTOSv202012.00/FreeRTOS/Source/queue.c
    FreeRTOSv202012.00/FreeRTOS/Source/stream_buffer.c
    FreeRTOSv202012.00/FreeRTOS/Source/tasks.c
    FreeRTOSv202012.00/FreeRTOS/Source/timers.c

    FreeRTOSv202012.00/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
    FreeRTOSv202012.00/FreeRTOS/Source/portable/MemMang/heap_3.c
)

set (MIDDLEWARE_INC
    cortex_m4_config
    FreeRTOSv202012.00/FreeRTOS/Source/include
    FreeRTOSv202012.00/FreeRTOS/Source/portable/GCC/ARM_CM3
)

target_include_directories(middleware INTERFACE
    ${MIDDLEWARE_INC}
)

target_include_directories(middleware PRIVATE
    ${MIDDLEWARE_INC}
)


target_link_libraries(middleware PRIVATE system)
