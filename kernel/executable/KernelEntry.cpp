#include <FunnyOS/Kernel/Kernel.hpp>

extern void* KERNEL_STACK_TOP;

namespace {
    extern "C" void start_kernel(FunnyOS::Bootparams::Parameters* parameters) {
        FunnyOS::Kernel::Kernel64::Get().Main(*parameters);
    }
}  // namespace

extern "C" void F_SECTION(".intro") F_NAKED fkrnl_entry(FunnyOS::Bootparams::Parameters*) {
#ifdef __GNUC__
    asm volatile(
        "pop %%rdi               \n"  // Pop parameters to RDI
        "mov %[new_stack], %%rsp \n"  // Setup new stack
        "mov %%rsp, %%rbp        \n"
        "jmp start_kernel        \n"
        :
        : [ new_stack ] "bN"(&KERNEL_STACK_TOP));
#endif
}