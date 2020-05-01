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
        fat_loader.asm
)

target_link_libraries(FunnyOS_Bootloader_FatLoader_ASM
        PUBLIC
            FunnyOS_Bootloader_Commons_ASM
)

target_compile_options(FunnyOS_Bootloader_FatLoader_ASM PRIVATE -f elf)

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
target_output_filename(FunnyOS_Bootloader_FatLoader fat_loader.bin)

target_link_options(FunnyOS_Bootloader_FatLoader
        PRIVATE
            -nostdlib -m32
)

target_compile_options(FunnyOS_Bootloader_FatLoader
        PRIVATE
            -m32 -Os -fno-pie -ffreestanding -fno-stack-protector
)

target_link_libraries(FunnyOS_Bootloader_FatLoader
        PUBLIC
            FunnyOS_Misc_QuickFat32
            FunnyOS_Bootloader_Commons_ASM
)
