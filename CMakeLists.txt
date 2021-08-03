cmake_minimum_required(VERSION 3.16)
project(target-cortexm LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_RTOS "Enable RTOS support" OFF)

message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

add_compile_options(
    -Wall
    -Wextra
    -Wconversion
    -Wsign-conversion
    $<$<CONFIG:DEBUG>:-g3>
    $<$<CONFIG:DEBUG>:-Og>
    $<$<CONFIG:RELEASE>:-O3>  
)

add_compile_definitions(
  $<$<CONFIG:DEBUG>:DEBUG>
  $<$<BOOL:${USE_RTOS}>:RTOS>
)

add_executable(Application 
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/devices.cpp
    ${CMAKE_SOURCE_DIR}/src/controller.cpp
)

target_include_directories(Application PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/include
)

set(LINKER_SCRIPTS 
    ${CMAKE_SOURCE_DIR}/ldscripts/mem.ld 
    ${CMAKE_SOURCE_DIR}/ldscripts/sections.ld
)

set_target_properties(Application PROPERTIES
    SUFFIX .elf
    LINK_DEPENDS "${LINKER_SCRIPTS}"
)

target_link_options(Application PRIVATE
  -T${CMAKE_SOURCE_DIR}/ldscripts/mem.ld
  -T${CMAKE_SOURCE_DIR}/ldscripts/sections.ld
  LINKER:--print-memory-usage
  LINKER:-Map,${CMAKE_CURRENT_BINARY_DIR}/Application.map
)

if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/system)
  add_subdirectory(system)
  target_link_libraries(Application PRIVATE system)
endif()

if (USE_RTOS AND IS_DIRECTORY ${CMAKE_SOURCE_DIR}/middleware)
  add_subdirectory(${CMAKE_SOURCE_DIR}/middleware)
  target_link_libraries(Application PRIVATE middleware)
endif()

if (EXISTS ${CMAKE_OBJCOPY})
  add_custom_command(
    TARGET Application
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:Application> 
            ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:Application>.hex
  )
else()
  message(STATUS "'objcopy' not found: cannot generate .hex file")
endif()

if (EXISTS "${TOOLCHAIN_SIZE}")
  add_custom_command(
    TARGET Application
    POST_BUILD
    COMMAND ${TOOLCHAIN_SIZE} --format=berkeley $<TARGET_FILE:Application> 
            >${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:Application>.bsz
  )

  add_custom_command(
    TARGET Application
    POST_BUILD
    COMMAND ${TOOLCHAIN_SIZE} --format=sysv -x $<TARGET_FILE:Application> 
            >${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:Application>.ssz
  )
else()
  message(STATUS "'size' not found: cannot generate .[bs]sz files")
endif()

