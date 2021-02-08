add_library(FunnyOS_Misc_TerminalManager STATIC
        src/TerminalManager.cpp
        src/TerminalManagerLoggingSink.cpp
)

target_include_directories(FunnyOS_Misc_TerminalManager
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
)

target_link_libraries(FunnyOS_Misc_TerminalManager
        PUBLIC
            FunnyOS_Stdlib_Base_Static_LL
)

