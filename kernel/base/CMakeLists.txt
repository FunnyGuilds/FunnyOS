add_library(FunnyOS_Kernel_Base STATIC
        src/KABI.cpp
        src/Kernel.cpp
        src/PlatformK64.cpp
)

target_include_directories(FunnyOS_Kernel_Base
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
)

target_link_libraries(FunnyOS_Kernel_Base
        PUBLIC
            FunnyOS_Stdlib_Base64_Kernel
            FunnyOS_Misc_Bootparams
            FunnyOS_Misc_Hardware64
            FunnyOS_Misc_TerminalManager64
)