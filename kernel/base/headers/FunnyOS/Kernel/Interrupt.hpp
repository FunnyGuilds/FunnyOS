#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_INTERRUPT_HPP
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_INTERRUPT_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>

namespace FunnyOS::Kernel {

    using Register = uint64_t;

    struct InterruptFrame {
        Register RAX;
        Register RCX;
        Register RDX;
        Register RBX;
        Register RBP;
        Register RSI;
        Register RDI;
        Register R8;
        Register R9;
        Register R10;
        Register R11;
        Register R12;
        Register R13;
        Register R14;
        Register R15;
        Register RIP;
        Register CS;
        Register RFLAGS;
        Register RSP;
        Register SS;
    };

}  // namespace FunnyOS::Kernel

#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_INTERRUPT_HPP
