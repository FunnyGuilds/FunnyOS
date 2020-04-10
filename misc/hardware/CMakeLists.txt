function(setup_library name)
    # Build objects
    add_library(${name}_ASM OBJECT
            src/InterruptSetup/interrupt_setup.asm
    )

    add_library(${name} STATIC
            src/InterruptSetup/InterruptSetup.cpp
            src/Interrupts.cpp
            src/VGA.cpp
            src/PIC.cpp
    )

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
    )


    target_link_libraries(${name} PRIVATE ${name}_ASM)
endfunction()

setup_library(FunnyOS_Misc_Hardware32)
target_compile_options(FunnyOS_Misc_Hardware32_ASM PRIVATE -f elf)
target_link_libraries(FunnyOS_Misc_Hardware32
        PUBLIC
            FunnyOS::Stdlib::Base32_Static
            FunnyOS::Misc::TerminalManager32
)
add_library(FunnyOS::Misc::Hardware32 ALIAS FunnyOS_Misc_Hardware32)

if (F_COMPILE64)
    setup_library(FunnyOS_Misc_Hardware64)
    target_compile_options(FunnyOS_Misc_Hardware64_ASM PRIVATE -f elf64)
    target_link_libraries(FunnyOS_Misc_Hardware64
            PUBLIC
                FunnyOS::Stdlib::Base64_Static
                FunnyOS::Misc::TerminalManager64
    )
    add_library(FunnyOS::Misc::Hardware64 ALIAS FunnyOS_Misc_Hardware64)
endif ()