function(setup_hw_library_variant name)
    add_library(${name} STATIC
            src/CMOS.cpp
            src/PS2.cpp
            src/Interrupts.cpp
            src/PIC.cpp
            src/Serial.cpp
            src/GFX/VGA.cpp
    )

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
    )
endfunction()

# 32-bit static version for bootloader
setup_hw_library_variant(FunnyOS_Misc_Hardware32)
target_link_libraries(FunnyOS_Misc_Hardware32
        PUBLIC
            FunnyOS_Stdlib_Base32_Static_LL
            FunnyOS_Misc_TerminalManager32
)

# 64-bit static version to be linked to kernel
setup_hw_library_variant(FunnyOS_Misc_Hardware64)
target_link_libraries(FunnyOS_Misc_Hardware64
        PUBLIC
            FunnyOS_Stdlib_Base64_Kernel
            FunnyOS_Misc_TerminalManager64
)
