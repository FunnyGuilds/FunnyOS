set(F_OUTPUT_IMAGE "${CMAKE_BINARY_DIR}/FunnyOS.img")

add_custom_command(
        OUTPUT ${F_OUTPUT_IMAGE}
        COMMAND /bin/bash ${CMAKE_SOURCE_DIR}/scripts/make-image.sh ${F_OUTPUT_IMAGE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS
            FunnyOS::Bootloader::MBR
            FunnyOS::Bootloader::Stage1
            FunnyOS::Bootloader::Bootloader32
)

add_custom_target(
        FunnyOS_BuildImage
        DEPENDS ${F_OUTPUT_IMAGE}
)

if (F_DEBUG_SYMBOLS)
    add_dependencies(FunnyOS_BuildImage FunnyOS_Bootloader_Bootloader32_DebugSymbols)
endif()