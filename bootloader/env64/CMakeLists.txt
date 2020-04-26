add_executable(FunnyOS_Bootloader_Env64
        env64.asm
)

set_target_properties(FunnyOS_Bootloader_Env64 PROPERTIES
        LINKER_LANGUAGE "CXX"
        PREFIX ""
        OUTPUT_NAME "env64"
        SUFFIX ".bin"
        COMPILE_OPTIONS "-f elf"
        LINK_OPTIONS "${F_BOOTLOADER_LINK_FLAGS};-T;${CMAKE_CURRENT_SOURCE_DIR}/env64.lds;-m32"
)

add_executable(FunnyOS::Bootloader::Env64 ALIAS FunnyOS_Bootloader_Env64)