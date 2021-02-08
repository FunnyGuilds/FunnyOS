# Config
configure_file(
        BootloaderConfig.hpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/config/BootloaderConfig.hpp
        @ONLY
)
configure_file(
        bootloader.lds.in
        ${CMAKE_CURRENT_BINARY_DIR}/bootloader.lds
        @ONLY
)

# Build ASM objects
add_library(FunnyOS_Bootloader_Bootloader_ASM OBJECT
        asm/dummy.asm
)

# Create .elf executable
add_executable(FunnyOS_Bootloader_Bootloader
        $<TARGET_OBJECTS:FunnyOS_Bootloader_Bootloader_ASM>
        src/Entry.cpp
)

target_include_directories(FunnyOS_Bootloader_Bootloader
        PUBLIC
            "${CMAKE_CURRENT_BINARY_DIR}/config/"
)

custom_link(FunnyOS_Bootloader_Bootloader "${CMAKE_CURRENT_BINARY_DIR}/bootloader.lds")
target_output_filename(FunnyOS_Bootloader_Bootloader bootloader.elf)
target_link_options(FunnyOS_Bootloader_Bootloader PRIVATE -Wl,--gc-sections)

target_link_libraries(FunnyOS_Bootloader_Bootloader
        PUBLIC
            FunnyOS_Stdlib_Base_Static_LL
            FunnyOS_Misc_Bootparams
            FunnyOS_Misc_Hardware
            FunnyOS_Misc_MemoryAllocator
            FunnyOS_Misc_TerminalManager
            FunnyOS_Misc_QuickFat
)

# .bin file target
add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/bootloader64.bin"
        COMMAND ${CMAKE_OBJCOPY} -O binary "$<TARGET_FILE:FunnyOS_Bootloader_Bootloader>" "${CMAKE_CURRENT_BINARY_DIR}/bootloader64.bin"
        DEPENDS
            FunnyOS_Bootloader_Bootloader
)

add_custom_target(
        FunnyOS_Bootloader_Bootloader_Binary
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/bootloader64.bin"
)
