# Build objects
add_library(FunnyOS_Bootloader_Bootloader32_ASM OBJECT
        asm/make_memory_map.asm
        asm/real_mode_intro.asm
        asm/start.asm
)
target_compile_options(FunnyOS_Bootloader_Bootloader32_ASM PRIVATE -f elf)

# Create .elf executable
add_executable(FunnyOS_Bootloader_Bootloader32
        $<TARGET_OBJECTS:FunnyOS_Bootloader_Bootloader32_ASM>
        src/A20Line.cpp
        src/Bootloader32.cpp
        src/BootloaderABI.cpp
        src/BootloaderStdlibPlatformImpl.cpp
        src/DebugMenu.cpp
        src/DebugMenuOptions.cpp
        src/ElfLoader.cpp
        src/Entry.cpp
        src/FileLoader.cpp
        src/Interrupts.cpp
        src/Logging.cpp
        src/Paging.cpp
        src/Sleep.cpp
)

set_target_properties(FunnyOS_Bootloader_Bootloader32 PROPERTIES
        CXX_STANDARD 20
        LINKER_LANGUAGE "CXX"
        PREFIX ""
        OUTPUT_NAME bootloader32
        SUFFIX .elf
        LINK_OPTIONS "${F_BOOTLOADER_LINK_FLAGS};-T;${CMAKE_CURRENT_SOURCE_DIR}/bootloader32.lds;-m32;-Wl,--gc-sections"
)

target_link_libraries(FunnyOS_Bootloader_Bootloader32
        PUBLIC
            FunnyOS::Stdlib::Base32_Static_LL
            FunnyOS::Misc::Bootparams
            FunnyOS::Misc::Hardware32
            FunnyOS::Misc::MemoryAllocator32
            FunnyOS::Misc::TerminalManager32
            FunnyOS::Misc::QuickFat32
            FunnyOS::Driver::Drive::BIOS32
)

add_executable(FunnyOS::Bootloader::Bootloader32 ALIAS FunnyOS_Bootloader_Bootloader32)

add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/bootloader32.bin"
        COMMAND ${CMAKE_OBJCOPY} -O binary "$<TARGET_FILE:FunnyOS_Bootloader_Bootloader32>" "${CMAKE_CURRENT_BINARY_DIR}/bootloader32.bin"
        DEPENDS
            FunnyOS::Bootloader::Bootloader32
)

add_custom_target(
        FunnyOS_Bootloader_Bootloader32_Binary
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/bootloader32.bin"
)