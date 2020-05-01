configure_file(
        config.asm.in
        ${CMAKE_CURRENT_BINARY_DIR}/config/config.asm
        @ONLY
)

add_library(FunnyOS_Bootloader_Commons_ASM OBJECT
        load_lba.asm
        print.asm
        ${CMAKE_CURRENT_BINARY_DIR}/config/config.asm
)

target_compile_options(FunnyOS_Bootloader_Commons_ASM
        PUBLIC
            -I "${CMAKE_CURRENT_BINARY_DIR}/config/"
        PRIVATE
            -f elf
)