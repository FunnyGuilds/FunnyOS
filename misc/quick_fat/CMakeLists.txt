function(setup_library name)
    add_library(${name} STATIC
            src/QuickFat.c
    )

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
    )

    target_compile_options(${name} PRIVATE -Os -fno-pie -ffreestanding -fno-stack-protector)
endfunction()

setup_library(FunnyOS_Misc_QuickFat32)
target_compile_options(FunnyOS_Misc_QuickFat32 PRIVATE -m32)
add_library(FunnyOS::Misc::QuickFat32 ALIAS FunnyOS_Misc_QuickFat32)

setup_library(FunnyOS_Misc_QuickFat64)
add_library(FunnyOS::Misc::QuickFat64 ALIAS FunnyOS_Misc_QuickFat64)