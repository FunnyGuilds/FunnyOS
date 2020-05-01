add_executable(FunnyOS_Bootloader_Stage1
        stage1.asm
)

target_link_libraries(FunnyOS_Bootloader_Stage1
        PUBLIC
            FunnyOS_Bootloader_Commons_ASM
)

custom_link(FunnyOS_Bootloader_Stage1 "${CMAKE_CURRENT_SOURCE_DIR}/stage1.lds")
target_output_filename(FunnyOS_Bootloader_Stage1 stage1.bin)

target_compile_options(FunnyOS_Bootloader_Stage1 PRIVATE -f elf)
target_link_options(FunnyOS_Bootloader_Stage1 PRIVATE -nostdlib -m32)
