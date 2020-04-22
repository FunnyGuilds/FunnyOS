configure_file(
        Config.hpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/config/FunnyOS/Stdlib/Config.hpp
        @ONLY
)

set(LL_FLAGS -O2 -fno-pie -ffunction-sections -fdata-sections -mno-sse)

function(setup_library name type)
    add_library(${name} STATIC
            src/Division.cpp
            src/Memory.cpp
            src/NewDelete.cpp
            src/String.cpp
            src/System.cpp)

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
                "${CMAKE_CURRENT_BINARY_DIR}/config"
    )

    target_compile_options(${name} PRIVATE -fpermissive)

    target_compile_definitions(${name} PRIVATE F_STDLIB_PLATFORM_EXTERN=1)
endfunction()

# Compile 32-bit base
setup_library(FunnyOS_Stdlib_Base32_Static STATIC)
target_compile_options(FunnyOS_Stdlib_Base32_Static PUBLIC -m32)
add_library(FunnyOS::Stdlib::Base32_Static ALIAS FunnyOS_Stdlib_Base32_Static)

# 32-bit base low level
setup_library(FunnyOS_Stdlib_Base32_Static_LL STATIC)
target_compile_options(FunnyOS_Stdlib_Base32_Static_LL PUBLIC -m32 ${LL_FLAGS})
add_library(FunnyOS::Stdlib::Base32_Static_LL ALIAS FunnyOS_Stdlib_Base32_Static_LL)

# Compile 64-bit base
setup_library(FunnyOS_Stdlib_Base64_Static STATIC)
target_compile_definitions(FunnyOS_Stdlib_Base64_Static PUBLIC F_64=1)
add_library(FunnyOS::Stdlib::Base64_Static ALIAS FunnyOS_Stdlib_Base64_Static)

# 64-bit base low level
setup_library(FunnyOS_Stdlib_Base64_Static_LL STATIC)
target_compile_options(FunnyOS_Stdlib_Base64_Static_LL PUBLIC ${LL_FLAGS})
add_library(FunnyOS::Stdlib::Base64_Static_LL ALIAS FunnyOS_Stdlib_Base64_Static_LL)

if (F_BUILD_TESTS)
    add_subdirectory("test")
endif()