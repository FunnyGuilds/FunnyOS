add_executable(FunnyOS_Bootloader_Stage1
        ./../commons/load_lba.asm
        ./../commons/print.asm
        stage1.asm
)

custom_link(FunnyOS_Bootloader_Stage1 "${CMAKE_CURRENT_SOURCE_DIR}/stage1.lds")
target_output_filename(FunnyOS_Bootloader_Stage1 stage1.bin)

target_compile_options(FunnyOS_Bootloader_Stage1 PRIVATE -f elf)
target_link_options(FunnyOS_Bootloader_Stage1 PRIVATE -nostdlib -m32)
