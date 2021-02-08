add_library(FunnyOS_Misc_QuickFat STATIC
        src/QuickFat.c
)

target_include_directories(FunnyOS_Misc_QuickFat
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
)

target_compile_options(FunnyOS_Misc_QuickFat PRIVATE
        -Os -ffreestanding -fno-stack-protector -fno-pie -mno-sse -mno-sse2 -mno-mmx -mno-80387
)

target_link_options(FunnyOS_Misc_QuickFat PRIVATE -nostdlib)

