function(setup_quickfat_variant name)
    add_library(${name} STATIC
            src/QuickFat.c
    )

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
    )

    target_compile_options(${name} PRIVATE -Os -ffreestanding -fno-stack-protector -fno-pie)
    target_link_options(${name} PRIVATE -nostdlib)
endfunction()

# 32-bit variant
setup_quickfat_variant(FunnyOS_Misc_QuickFat32)
target_compile_options(FunnyOS_Misc_QuickFat32 PRIVATE -m32)

# 64-bit variant
setup_quickfat_variant(FunnyOS_Misc_QuickFat64)
