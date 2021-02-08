#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>

extern "C" {

static void bootloader64_call_real_main() {
    for (;;) {
    }
}

// Entry-point
[[noreturn]] F_SECTION(".intro") F_NAKED void bootloader64_main(uint8_t, uint8_t, uintptr_t) {
    asm("mov %rsp, %rdx                     \n\t"
        "xor %rbp, %rbp                     \n\t"
        "jmp bootloader64_call_real_main    \n\t");
}
}