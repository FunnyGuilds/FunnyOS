add_executable(FunnyOS_Kernel_Executable KernelEntry.cpp)

configure_file(
        kernel.lds.in
        ${CMAKE_CURRENT_BINARY_DIR}/kernel.lds
        @ONLY
)

target_output_filename(FunnyOS_Kernel_Executable fkrnl.fxe)
custom_link(FunnyOS_Kernel_Executable "${CMAKE_CURRENT_BINARY_DIR}/kernel.lds")

target_link_libraries(FunnyOS_Kernel_Executable
        PUBLIC
            FunnyOS_Kernel_Base
            FunnyOS_Stdlib_Base64_Kernel
            FunnyOS_Kernel_Base              # Again to deal with linking order and external _Platform functions
)