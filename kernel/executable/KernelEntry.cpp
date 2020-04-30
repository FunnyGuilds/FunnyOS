#include <FunnyOS/Kernel/Kernel.hpp>

#include <FunnyOS/Stdlib/System.hpp>

extern void* KERNEL_STACK_TOP;

extern "C" void F_SECTION(".intro") F_NAKED fkrnl_entry() {
    FunnyOS::Bootparams::Parameters* parameters;

#ifdef __GNUC__
    asm volatile(
        "pop %0\n"       // Pop parameters
        "mov rsp, %1\n"  // Setup new stack
        "mov rbp, rsp\n"
        : "=r"(parameters)
        : "bN"(&KERNEL_STACK_TOP)
        : "rbp");
#endif

    FunnyOS::Kernel::Kernel64::Get().Initialize(*parameters);
}