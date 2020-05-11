#include <FunnyOS/Kernel/Kernel.hpp>

#include <FunnyOS/Stdlib/System.hpp>

extern void* KERNEL_STACK_TOP;

namespace {
    extern "C" void start_kernel(FunnyOS::Bootparams::Parameters* parameters) {
        FunnyOS::Kernel::Kernel64::Get().Main(*parameters);
    }
}  // namespace

extern "C" void F_SECTION(".intro") F_NAKED fkrnl_entry() {
#ifdef __GNUC__
    asm volatile(
        "pop %%rdi\n"          // Pop parameters
        "mov %0, %%rsp\n"      // Setup new stack
        "mov %%rsp, %%rbp\n"
        "jmp start_kernel"
        :
        : "bN"(&KERNEL_STACK_TOP)
        : "rbp");
#endif
}