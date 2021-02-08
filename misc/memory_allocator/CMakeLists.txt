add_library(FunnyOS_Misc_MemoryAllocator STATIC
        src/StaticMemoryAllocator.cpp
)

target_include_directories(FunnyOS_Misc_MemoryAllocator
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/headers/"
)

target_link_libraries(FunnyOS_Misc_MemoryAllocator
        PUBLIC
            FunnyOS_Stdlib_Base_Static_LL
)
