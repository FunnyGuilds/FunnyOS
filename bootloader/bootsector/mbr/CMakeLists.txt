add_executable(FunnyOS_Bootloader_MBR
        mbr.asm
)

target_link_libraries(FunnyOS_Bootloader_MBR
        PUBLIC
            FunnyOS_Bootloader_Commons_ASM
)

custom_link(FunnyOS_Bootloader_MBR "${CMAKE_CURRENT_SOURCE_DIR}/mbr.lds")
target_output_filename(FunnyOS_Bootloader_MBR mbr.bin)

target_link_options(FunnyOS_Bootloader_MBR PRIVATE -nostdlib)

