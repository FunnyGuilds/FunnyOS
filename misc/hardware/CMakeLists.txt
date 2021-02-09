add_library(FunnyOS_Misc_Hardware STATIC
        src/CMOS.cpp
        src/GDT.cpp
        src/Interrupts.cpp
        src/PIC.cpp
        src/PS2.cpp
        src/Serial.cpp
        src/GFX/FramebufferInterface.cpp
        src/GFX/FontTerminalInterface.cpp
        src/GFX/VGA.cpp
)

target_include_directories(FunnyOS_Misc_Hardware
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
)

target_link_libraries(FunnyOS_Misc_Hardware
        PUBLIC
            FunnyOS_Stdlib_Base_Static_LL
            FunnyOS_Misc_TerminalManager
)
