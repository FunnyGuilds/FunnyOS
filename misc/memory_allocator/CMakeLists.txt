function(setup_memory_allocator_variant name)
    add_library(${name} STATIC
            src/StaticMemoryAllocator.cpp
    )

    target_include_directories(${name}
            PUBLIC
                "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
    )
endfunction()

# 32-bit static version for bootloader
setup_memory_allocator_variant(FunnyOS_Misc_MemoryAllocator32)
target_link_libraries(FunnyOS_Misc_MemoryAllocator32
        PUBLIC
            FunnyOS_Stdlib_Base32_Static_LL
)

# 64-bit static version to be linked to kernel
setup_memory_allocator_variant(FunnyOS_Misc_MemoryAllocator64)
target_link_libraries(FunnyOS_Misc_MemoryAllocator64
        PUBLIC
            FunnyOS_Stdlib_Base64_Kernel
)
