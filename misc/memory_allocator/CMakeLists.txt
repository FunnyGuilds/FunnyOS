function(setup_library name type)
    add_library(${name} ${type}
            src/StaticMemoryAllocator.cpp
    )

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers"
    )
endfunction()

setup_library(FunnyOS_Misc_MemoryAllocator32 STATIC)
target_link_libraries(FunnyOS_Misc_MemoryAllocator32 PUBLIC FunnyOS::Stdlib::Base32_Static_LL)
add_library(FunnyOS::Misc::MemoryAllocator32 ALIAS FunnyOS_Misc_MemoryAllocator32)

setup_library(FunnyOS_Misc_MemoryAllocator64 SHARED)
target_link_libraries(FunnyOS_Misc_MemoryAllocator64 PUBLIC FunnyOS::Stdlib::Base64_Kernel)
add_library(FunnyOS::Misc::MemoryAllocator64 ALIAS FunnyOS_Misc_MemoryAllocator64)