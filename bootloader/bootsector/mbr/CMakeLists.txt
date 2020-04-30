add_executable(FunnyOS_Bootloader_MBR
        ./../commons/load_lba.asm
        ./../commons/print.asm
        mbr.asm
)

custom_link(FunnyOS_Bootloader_MBR "${CMAKE_CURRENT_SOURCE_DIR}/mbr.lds")
target_output_filename(FunnyOS_Bootloader_MBR mbr.bin)

target_compile_options(FunnyOS_Bootloader_MBR PRIVATE -f elf)
target_link_options(FunnyOS_Bootloader_MBR PRIVATE -nostdlib -m32)
