add_executable(FunnyOS_Bootloader_MBR
        ./../commons/load_lba.asm
        ./../commons/print.asm
        mbr.asm
)
set_target_properties(FunnyOS_Bootloader_MBR PROPERTIES
        LINKER_LANGUAGE "CXX"
        PREFIX ""
        OUTPUT_NAME "mbr"
        SUFFIX ".bin"
        COMPILE_OPTIONS "-f elf"
        LINK_OPTIONS "${F_BOOTLOADER_LINK_FLAGS};-T;${CMAKE_CURRENT_SOURCE_DIR}/mbr.lds;-m32")

add_executable(FunnyOS::Bootloader::MBR ALIAS FunnyOS_Bootloader_MBR)