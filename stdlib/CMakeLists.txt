configure_file(
        Config.hpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/config/FunnyOS/Stdlib/Config.hpp
        @ONLY
)

function(setup_stdlib_variant name type)
    add_library(${name} ${type}
            src/File.cpp
            src/Logging.cpp
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

    list(FIND ARGN "LL" LL_index)
    if (NOT LL_index EQUAL -1)
        target_compile_definitions(${name} PUBLIC F_LL=1)
        target_compile_options(${name} PUBLIC
                -O0
                -mno-sse
                -mno-sse2
                -mno-mmx
                -mno-80387
                -fno-exceptions
                -fno-rtti
                -fno-stack-protector
                -mcmodel=large
        )

        target_compile_options(${name} PRIVATE -fdata-sections -ffunction-sections)
    endif()
endfunction()

# Compile 64-bit base, for user-space programs
setup_stdlib_variant(FunnyOS_Stdlib_Base_Static STATIC)

# Compile 64-bit base, for test
setup_stdlib_variant(FunnyOS_Stdlib_Base_Static_Test STATIC TESTS)

# 64-bit base for kernel, without some user-space only features
setup_stdlib_variant(FunnyOS_Stdlib_Base_Static_LL STATIC LL)

if (F_BUILD_TESTS)
    add_subdirectory("test")
endif()
