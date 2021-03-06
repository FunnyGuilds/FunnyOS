# Configs
configure_file(
        fat_loader.lds.in
        ${CMAKE_CURRENT_BINARY_DIR}/fat_loader.lds
        @ONLY
)

configure_file(
        config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/config/config.h
        @ONLY
)

# .asm files
add_library(FunnyOS_Bootloader_FatLoader_ASM OBJECT
        intro.asm
        bios_disk_interface.asm
)

target_link_libraries(FunnyOS_Bootloader_FatLoader_ASM
        PUBLIC
            FunnyOS_Bootloader_Commons_ASM
)

# Executable
add_executable(FunnyOS_Bootloader_FatLoader
        $<TARGET_OBJECTS:FunnyOS_Bootloader_FatLoader_ASM>
        fat_loader.c
)

target_include_directories(FunnyOS_Bootloader_FatLoader
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/config/"
)

custom_link(FunnyOS_Bootloader_FatLoader "${CMAKE_CURRENT_BINARY_DIR}/fat_loader.lds")
target_output_filename(FunnyOS_Bootloader_FatLoader fat_loader.elf)

target_link_options(FunnyOS_Bootloader_FatLoader
        PRIVATE
            -nostdlib
)

target_compile_options(FunnyOS_Bootloader_FatLoader
        PRIVATE
            -Os -fno-pie -ffreestanding -fno-stack-protector -mno-sse -mno-sse2 -mno-mmx -mno-80387
)

target_link_libraries(FunnyOS_Bootloader_FatLoader
        PUBLIC
            FunnyOS_Misc_QuickFat
            FunnyOS_Bootloader_Commons_ASM
)

# .bin file target
add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/fat_loader.bin"
        COMMAND ${CMAKE_OBJCOPY} -O binary "$<TARGET_FILE:FunnyOS_Bootloader_FatLoader>" "${CMAKE_CURRENT_BINARY_DIR}/fat_loader.bin"
        DEPENDS
        FunnyOS_Bootloader_FatLoader
)

add_custom_target(
        FunnyOS_Bootloader_FatLoader_Binary
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/fat_loader.bin"
)