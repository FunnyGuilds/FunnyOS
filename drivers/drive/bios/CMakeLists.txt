function(setup_library name)
    add_library(${name} STATIC
            src/BiosDriveInterface.cpp
    )

    target_include_directories(${name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/headers")
endfunction()

setup_library(FunnyOS_Driver_Drive_BIOS32)

target_link_libraries(FunnyOS_Driver_Drive_BIOS32
        PUBLIC
            FunnyOS::Misc::Hardware32
)

add_library(FunnyOS::Driver::Drive::BIOS32 ALIAS FunnyOS_Driver_Drive_BIOS32)

setup_library(FunnyOS_Driver_Drive_BIOS64)
target_link_libraries(FunnyOS_Misc_Hardware64
        PUBLIC
            FunnyOS::Misc::Hardware64
)
add_library(FunnyOS::Driver::Drive::BIOS64 ALIAS FunnyOS_Driver_Drive_BIOS64)