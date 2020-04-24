function(setup_library name type)
    add_library(${name} ${type}
            src/TerminalManager.cpp
    )

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
    )
endfunction()

setup_library(FunnyOS_Misc_TerminalManager32 STATIC)
target_link_libraries(FunnyOS_Misc_TerminalManager32 PUBLIC FunnyOS::Stdlib::Base32_Static_LL)
add_library(FunnyOS::Misc::TerminalManager32 ALIAS FunnyOS_Misc_TerminalManager32)

setup_library(FunnyOS_Misc_TerminalManager64 SHARED)
target_link_libraries(FunnyOS_Misc_TerminalManager64 PUBLIC FunnyOS::Stdlib::Base64_Kernel)
add_library(FunnyOS::Misc::TerminalManager64 ALIAS FunnyOS_Misc_TerminalManager64)