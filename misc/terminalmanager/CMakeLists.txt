function(setup_terminal_manager_variant name)
    add_library(${name} STATIC
            src/TerminalManager.cpp
    )

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
    )
endfunction()

# 32-bit static version for bootloader
setup_terminal_manager_variant(FunnyOS_Misc_TerminalManager32)
target_link_libraries(FunnyOS_Misc_TerminalManager32
        PUBLIC
            FunnyOS_Stdlib_Base32_Static_LL
)

# 64-bit static version to be linked to kernel
setup_terminal_manager_variant(FunnyOS_Misc_TerminalManager64)
target_link_libraries(FunnyOS_Misc_TerminalManager64
        PUBLIC
            FunnyOS_Stdlib_Base64_Kernel
)
