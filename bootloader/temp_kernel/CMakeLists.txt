add_executable(FunnyOS_Bootloader_TempKernel
        temp_kernel.asm
)

custom_link(FunnyOS_Bootloader_TempKernel temp_kernel.lds)
target_output_filename(FunnyOS_Bootloader_TempKernel temp_kernel.bin)
target_compile_options(FunnyOS_Bootloader_TempKernel PRIVATE -f elf64)
target_link_options(FunnyOS_Bootloader_TempKernel PRIVATE -nostdlib)
