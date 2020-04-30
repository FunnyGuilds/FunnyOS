add_library(FunnyOS_Bootloader_FatLoader_ASM OBJECT
        ./../commons/load_lba.asm
        fat_loader.asm
)
target_compile_options(FunnyOS_Bootloader_FatLoader_ASM PRIVATE -f elf)

add_executable(FunnyOS_Bootloader_FatLoader
        $<TARGET_OBJECTS:FunnyOS_Bootloader_FatLoader_ASM>
        fat_loader.c
)

custom_link(FunnyOS_Bootloader_FatLoader "${CMAKE_CURRENT_SOURCE_DIR}/fat_loader.lds")
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
)
