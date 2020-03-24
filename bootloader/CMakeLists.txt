if (NOT ${CMAKE_GENERATOR} STREQUAL "Unix Makefiles" AND NOT ${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
    message(FATAL_ERROR "Unsupported generator")
endif ()

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bootloader.bin
        COMMAND ${CMAKE_MAKE_PROGRAM} OUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR} CAT=${CAT_CMD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS
            stage1/stage1.asm
            stage2/protected/cpuid.asm
            stage2/protected/gdt.asm
            stage2/protected/long.asm
            stage2/protected/paging.asm
            stage2/protected/protected.asm
            stage2/protected/screen.asm
            stage2/a20line.asm
            stage2/drive.asm
            stage2/gdt.asm
            stage2/logging.asm
            stage2/stage2.asm
            common.asm
            data.asm
)

add_custom_target(KekOS_Bootloader DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bootloader.bin)