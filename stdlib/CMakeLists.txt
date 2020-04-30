configure_file(
        Config.hpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/config/FunnyOS/Stdlib/Config.hpp
        @ONLY
)

function(setup_stdlib_variant name type)
    add_library(${name} ${type}
            src/Division.cpp
            src/Memory.cpp
            src/NewDelete.cpp
            src/String.cpp
            src/System.cpp
    )

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
                "${CMAKE_CURRENT_BINARY_DIR}/config/"
    )

    target_compile_options(${name} PRIVATE -fpermissive)
    target_compile_definitions(${name} PRIVATE F_STDLIB_PLATFORM_EXTERN=1)

    # Special flags
    list(FIND ARGN "TESTS" tests_index)
    if (tests_index EQUAL -1)
        target_compile_options(${name} PUBLIC -ffreestanding)
        target_link_options(${name} PUBLIC -nostdlib)
    endif()

    list(FIND ARGN "64" 64_index)
    if (NOT 64_index EQUAL -1)
        target_compile_definitions(${name} PUBLIC F_64=1)
    endif()

    list(FIND ARGN "LL" LL_index)
    if (NOT LL_index EQUAL -1)
        target_compile_definitions(${name} PUBLIC F_LL=1)
        target_compile_options(${name} PUBLIC -mno-sse -fno-exceptions -fno-rtti)
    endif()
endfunction()

# 32-bit base low level, for low-level startup code (mainly bootloader and its modules)
setup_stdlib_variant(FunnyOS_Stdlib_Base32_Static_LL STATIC LL)
target_compile_options(FunnyOS_Stdlib_Base32_Static_LL PUBLIC -m32 -O2)

# Compile 64-bit base, for user-space programs
setup_stdlib_variant(FunnyOS_Stdlib_Base64_Static STATIC 64)

# Compile 64-bit base, for test
setup_stdlib_variant(FunnyOS_Stdlib_Base64_Static_Test STATIC TESTS 64)

# 64-bit base for kernel, without some user-space only features
setup_stdlib_variant(FunnyOS_Stdlib_Base64_Kernel STATIC 64 LL)
target_compile_definitions(FunnyOS_Stdlib_Base64_Kernel PUBLIC F_KERNEL=1)
target_compile_options(FunnyOS_Stdlib_Base64_Kernel PUBLIC -O2)

if (F_BUILD_TESTS)
    add_subdirectory("test")
endif()
