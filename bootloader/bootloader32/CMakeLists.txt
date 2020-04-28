# Build ASM objects
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

custom_link(FunnyOS_Bootloader_Bootloader32 bootloader32.lds)
target_output_filename(FunnyOS_Bootloader_Bootloader32 bootloader32.elf)
target_link_options(FunnyOS_Bootloader_Bootloader32 PRIVATE -m32 -Wl,--gc-sections)

target_link_libraries(FunnyOS_Bootloader_Bootloader32
        PUBLIC
            FunnyOS_Stdlib_Base32_Static_LL
            FunnyOS_Misc_Bootparams
            FunnyOS_Misc_Hardware32
            FunnyOS_Misc_MemoryAllocator32
            FunnyOS_Misc_TerminalManager32
            FunnyOS_Misc_QuickFat32
            FunnyOS_Driver_Drive_BIOS32
)


# .bin file target
add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/bootloader32.bin"
        COMMAND ${CMAKE_OBJCOPY} -O binary "$<TARGET_FILE:FunnyOS_Bootloader_Bootloader32>" "${CMAKE_CURRENT_BINARY_DIR}/bootloader32.bin"
        DEPENDS
            FunnyOS_Bootloader_Bootloader32
)

add_custom_target(
        FunnyOS_Bootloader_Bootloader32_Binary
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/bootloader32.bin"
)
