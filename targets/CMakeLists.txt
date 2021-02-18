set(F_OUTPUT_IMAGE              "${CMAKE_BINARY_DIR}/FunnyOS.img")
set(F_BOOTLOADER_INI_SOURCE     "${CMAKE_BINARY_DIR}/bootload64.ini")

# bootload.ini file config
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/files/bootload64.ini"
        "${F_BOOTLOADER_INI_SOURCE}"
        @ONLY
)

# build-info.sh config
configure_file(
        "${CMAKE_SOURCE_DIR}/targets/build-info.sh.in"
        "${CMAKE_BINARY_DIR}/build-info.sh"
        @ONLY
)

add_custom_command(
        OUTPUT ${F_OUTPUT_IMAGE}
        COMMAND /bin/bash -xe ${CMAKE_SOURCE_DIR}/scripts/make-image.sh ${F_OUTPUT_IMAGE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS
            FunnyOS_Bootloader_MBR
            FunnyOS_Bootloader_Stage1
            FunnyOS_Bootloader_FatLoader
            FunnyOS_Bootloader_FatLoader_Binary
            FunnyOS_Bootloader_Bootloader
            FunnyOS_Bootloader_Bootloader_Binary
            FunnyOS_Kernel_Executable
)

add_custom_target(
        FunnyOS_BuildImage
        DEPENDS ${F_OUTPUT_IMAGE}
)
