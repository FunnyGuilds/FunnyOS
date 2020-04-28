add_executable(FunnyOS_Bootloader_Env64
        env64.asm
)

custom_link(FunnyOS_Bootloader_Env64 env64.lds)
target_output_filename(FunnyOS_Bootloader_Env64 env64.bin)
target_compile_options(FunnyOS_Bootloader_Env64 PRIVATE -f elf)
target_link_options(FunnyOS_Bootloader_Env64 PRIVATE -nostdlib -m32)
