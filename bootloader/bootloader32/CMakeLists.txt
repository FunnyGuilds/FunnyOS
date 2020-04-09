# Build objects
add_library(FunnyOS_Bootloader_Bootloader32_ASM OBJECT
        asm/make_memory_map.asm
        asm/real_mode_intro.asm
        asm/start.asm
)
target_compile_options(FunnyOS_Bootloader_Bootloader32_ASM PRIVATE -f elf)

function(setup name link_script file_name suffix)
    add_executable(${name}
        $<TARGET_OBJECTS:FunnyOS_Bootloader_Bootloader32_ASM>
        src/Entry.cpp
        src/A20Line.cpp
        src/Interrupts.cpp
        src/Bootloader32.cpp
    )

    set_target_properties(${name} PROPERTIES
            CXX_STANDARD 20
            LINKER_LANGUAGE "CXX"
            PREFIX ""
            OUTPUT_NAME ${file_name}
            SUFFIX ${suffix}
            LINK_OPTIONS "${F_BOOTLOADER_LINK_FLAGS};-T;${CMAKE_CURRENT_SOURCE_DIR}/${link_script};-m32"
    )

    target_compile_options(${name} PUBLIC -Os -m32 -fno-pie)

    target_link_libraries(${name}
            PUBLIC
                FunnyOS::Stdlib::Base32_Static
                FunnyOS::Bootloader::Commons32
    )
endfunction()


# Create .bin executable
setup(FunnyOS_Bootloader_Bootloader32 "bootloader32.lds" bootloader32 .bin)

# Create .elf executable if debug symbols are enabled
if (F_DEBUG_SYMBOLS)
    setup(FunnyOS_Bootloader_Bootloader32_DebugSymbols "bootloader32_debug.lds" bootloader32-debug-symbols .elf)
    add_executable(FunnyOS::Bootloader::Bootloader32::DebugSymbols ALIAS FunnyOS_Bootloader_Bootloader32_DebugSymbols)
endif ()

add_executable(FunnyOS::Bootloader::Bootloader32 ALIAS FunnyOS_Bootloader_Bootloader32)