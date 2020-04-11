#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_HPP
#error "Include Interrupts.hpp instaed"
#endif
#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_TCC
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_TCC

#include "CPU.hpp"

namespace FunnyOS::HW {
    inline void EnableHardwareInterrupts() {
#ifdef __GNUC__
        asm volatile("sti");
#endif
    }

    inline void DisableHardwareInterrupts() {
#ifdef __GNUC__
        asm volatile("cli");
#endif
    }

    inline bool HardwareInterruptsEnabled() {
        return (CPU::GetFlagsRegister() & CPU::Flags::InterruptFlag) != 0;
    }
}  // namespace FunnyOS::HW

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_TCC
