function(setup_hw_library_variant name)
    # Build objects
    add_library(${name}_ASM STATIC
            src/InterruptSetup/interrupt_setup.asm
            src/real_mode.asm
    )

    add_library(${name} STATIC
            src/InterruptSetup/InterruptSetup.cpp
            src/CMOS.cpp
            src/PS2.cpp
            src/RealMode.cpp
            src/Interrupts.cpp
            src/PIC.cpp
            src/Serial.cpp
            src/VGA.cpp
    )

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
    )

    target_link_libraries(${name} PRIVATE ${name}_ASM)
    target_compile_options(${name}_ASM PRIVATE -I "${CMAKE_CURRENT_SOURCE_DIR}/src")
endfunction()

# 32-bit static version for bootloader
setup_hw_library_variant(FunnyOS_Misc_Hardware32)
target_compile_options(FunnyOS_Misc_Hardware32_ASM PRIVATE -f elf)
target_link_libraries(FunnyOS_Misc_Hardware32
        PUBLIC
            FunnyOS_Stdlib_Base32_Static_LL
            FunnyOS_Misc_TerminalManager32
)

# 64-bit static PIC version to be linked as a kernel module later
setup_hw_library_variant(FunnyOS_Misc_Hardware64)
target_compile_options(FunnyOS_Misc_Hardware64_ASM PRIVATE -f elf64)
target_link_libraries(FunnyOS_Misc_Hardware64
        PUBLIC
            FunnyOS_Stdlib_Base64_Kernel
            FunnyOS_Misc_TerminalManager64
)
