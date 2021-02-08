cmake_minimum_required(VERSION 3.16)
project(FunnyOS C CXX ASM_NASM)
include(Config.cmake)

# CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Linking)

# Checks
if (NOT ${CMAKE_GENERATOR} STREQUAL "Unix Makefiles" AND NOT ${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
    message(FATAL_ERROR "Unsupported generator")
endif ()

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "Unsupported compiler")
endif ()

# Options
option(F_BUILD_TESTS      "Should the testes be built?"       ON)

# Tests
if (F_BUILD_TESTS)
    find_package(GTest)

    if (GTEST_FOUND)
        enable_testing()
    else()
        message(WARNING "Testing will not be enabled as googletest was not found")
        set(F_BUILD_TESTS OFF)
    endif()
endif()

# Global variables
set(WARNING_OPTIONS
        "-Wall -Wextra -Wno-unknown-pragmas -Wno-unused-function -Wno-unused-const-variable -Wno-c99-designator"
)

# C and C++ standards
set(CMAKE_CXX_STANDARD 20)

# Global flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_OPTIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_OPTIONS}")

set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -f elf64 -o <OBJECT> <SOURCE>")

# Default build type
if (CMAKE_BUILD_TYPE EQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# Global definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DF_DEBUG)
endif ()

add_subdirectory("stdlib")
add_subdirectory("misc")
add_subdirectory("bootloader")
add_subdirectory("kernel")
add_subdirectory("scripts")
add_subdirectory("targets")
