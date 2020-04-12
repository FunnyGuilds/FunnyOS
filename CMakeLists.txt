cmake_minimum_required(VERSION 3.17)
project(FunnyOS CXX ASM_NASM)

# Version
set(FUNNYOS_VERSION 1.0.0)

# Checks
if (NOT ${CMAKE_GENERATOR} STREQUAL "Unix Makefiles" AND NOT ${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
    message(FATAL_ERROR "Unsupported generator")
endif ()

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "Unsupported compiler")
endif ()

# Options
option(F_COMPILE64        "Build FunnyOS in 64-bit mode?"     ON)
option(F_BUILD_TESTS      "Should the testes be built?"       ON)

if (NOT F_COMPILE64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif ()

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
        "-Wall -Wextra -Wno-unknown-pragmas -Wno-unused-function"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -masm=intel -no-pie ${WARNING_OPTIONS}")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")

if (CMAKE_BUILD_TYPE EQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(F_DEBUG_SYMBOLS TRUE)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DF_DEBUG)
endif ()

add_subdirectory("stdlib")
add_subdirectory("misc")
add_subdirectory("bootloader")
add_subdirectory("targets")