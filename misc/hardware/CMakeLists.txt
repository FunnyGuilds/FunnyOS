function(setup_library name type)
    # Build objects
    add_library(${name}_ASM STATIC
            src/InterruptSetup/interrupt_setup.asm
            src/real_mode.asm
    )

    add_library(${name} ${type}
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

setup_library(FunnyOS_Misc_Hardware32 STATIC)
target_compile_options(FunnyOS_Misc_Hardware32_ASM PRIVATE -f elf)
target_link_libraries(FunnyOS_Misc_Hardware32
        PUBLIC
            FunnyOS::Stdlib::Base32_Static_LL
            FunnyOS::Misc::TerminalManager32
)
add_library(FunnyOS::Misc::Hardware32 ALIAS FunnyOS_Misc_Hardware32)

setup_library(FunnyOS_Misc_Hardware64 SHARED)
target_compile_options(FunnyOS_Misc_Hardware64_ASM PRIVATE -f elf64)
target_link_libraries(FunnyOS_Misc_Hardware64
        PUBLIC
            FunnyOS::Stdlib::Base64_Kernel
            FunnyOS::Misc::TerminalManager64
)
add_library(FunnyOS::Misc::Hardware64 ALIAS FunnyOS_Misc_Hardware64)