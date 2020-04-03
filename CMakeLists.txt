cmake_minimum_required(VERSION 3.17)
project(FunnyOS CXX ASM_NASM)

# Checks
if (NOT ${CMAKE_GENERATOR} STREQUAL "Unix Makefiles" AND NOT ${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
    message(FATAL_ERROR "Unsupported generator")
endif ()

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "Unsupported compiler")
endif ()

# Options
option(FOS_COMPILE64 "Build FOS in 64-bit mode?" On)

if (NOT FOS_COMPILE64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif ()

# Global variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -masm=intel -no-pie")

if (CMAKE_BUILD_TYPE EQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(FOS_DEBUG_SYMBOLS TRUE)
endif ()

add_subdirectory("stdlib")
add_subdirectory("misc")
add_subdirectory("bootloader")
add_subdirectory("targets")