add_executable(FunnyOS_Bootloader_Stage1
        ./../commons/load_lba.asm
        ./../commons/print.asm
        stage1.asm
        )

set_target_properties(FunnyOS_Bootloader_Stage1 PROPERTIES
        LINKER_LANGUAGE "CXX"
        PREFIX ""
        OUTPUT_NAME "stage1"
        SUFFIX ".bin"
        LINK_FLAGS "${FUNNYOS_BOOTLOADER_LINK_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/stage1.lds")

add_executable(FunnyOS::Bootloader::Stage1 ALIAS FunnyOS_Bootloader_Stage1)