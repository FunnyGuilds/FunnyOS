add_library(FunnyOS_Bootloader_FatLoader_ASM OBJECT
        ./../commons/load_lba.asm
        fat_loader.asm
)
target_compile_options(FunnyOS_Bootloader_FatLoader_ASM PRIVATE -f elf)

add_executable(FunnyOS_Bootloader_FatLoader
        $<TARGET_OBJECTS:FunnyOS_Bootloader_FatLoader_ASM>
        fat_loader.c
)

set_target_properties(FunnyOS_Bootloader_FatLoader PROPERTIES
        C_STANDARD 11
        LINKER_LANGUAGE "CXX"
        PREFIX ""
        OUTPUT_NAME "fat_loader"
        SUFFIX ".bin"
        COMPILE_OPTIONS "-m32;-Os;-fno-pie;-ffreestanding;-fno-stack-protector"
        LINK_OPTIONS "${F_BOOTLOADER_LINK_FLAGS};-T;${CMAKE_CURRENT_SOURCE_DIR}/fat_loader.lds;-m32"
)

target_link_libraries(FunnyOS_Bootloader_FatLoader PUBLIC FunnyOS::Misc::QuickFat32)

add_executable(FunnyOS::Bootloader::FatLoader ALIAS FunnyOS_Bootloader_FatLoader)