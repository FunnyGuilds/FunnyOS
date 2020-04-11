set(SOURCES
        src/BootloaderStdlibPlatformImpl.cpp
        src/Logging.cpp
        src/LowMemoryAllocator.cpp
        src/BootloaderABI.cpp
        src/Bootloader.cpp
        src/Sleep.cpp
)

function(setup_library name)
    add_library(${name} STATIC ${SOURCES})

    target_compile_options(${name} PUBLIC -Os -fno-pie)

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers"
    )
endfunction()

setup_library(FunnyOS_Bootloader_Commons32)
target_compile_options(FunnyOS_Bootloader_Commons32 PUBLIC -m32)
target_link_libraries(FunnyOS_Bootloader_Commons32
        PUBLIC
            FunnyOS::Stdlib::Base32_Static
            FunnyOS::Misc::Hardware32
            FunnyOS::Misc::TerminalManager32
)
add_library(FunnyOS::Bootloader::Commons32 ALIAS FunnyOS_Bootloader_Commons32)

if(F_COMPILE64)
    setup_library(FunnyOS_Bootloader_Commons64)
    target_link_libraries(FunnyOS_Bootloader_Commons64
            PUBLIC
                FunnyOS::Stdlib::Base64_Static
                FunnyOS::Misc::Hardware64
                FunnyOS::Misc::TerminalManager64
    )
    add_library(FunnyOS::Bootloader::Commons64 ALIAS FunnyOS_Bootloader_Commons64)
endif()