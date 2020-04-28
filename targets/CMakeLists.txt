set(F_OUTPUT_IMAGE "${CMAKE_BINARY_DIR}/FunnyOS.img")

add_custom_command(
        OUTPUT ${F_OUTPUT_IMAGE}
        COMMAND /bin/bash -xe ${CMAKE_SOURCE_DIR}/scripts/make-image.sh ${F_OUTPUT_IMAGE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS
            FunnyOS_Bootloader_MBR
            FunnyOS_Bootloader_Stage1
            FunnyOS_Bootloader_FatLoader
            FunnyOS_Bootloader_Bootloader32
            FunnyOS_Bootloader_Bootloader32_Binary
            FunnyOS_Bootloader_Env64
            FunnyOS_Bootloader_TempKernel
)

add_custom_target(
        FunnyOS_BuildImage
        DEPENDS ${F_OUTPUT_IMAGE}
)
