#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_HPP
#error "Include Interrupts.hpp instaed"
#endif
#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_TCC
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_TCC

#include "CPU.hpp"
#include "InputOutput.hpp"

namespace FunnyOS::HW {
    namespace {
        constexpr const uint16_t CMOS_PORT = 0x70;
        constexpr const uint8_t NMI_DISABLE_FLAG = 0b1000000;
    }  // namespace

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

    inline void EnableNonMaskableInterrupts() {
        InputOutput::OutputByte(CMOS_PORT, InputOutput::InputByte(CMOS_PORT) & (~NMI_DISABLE_FLAG));
    }

    inline void DisableNonMaskableInterrupts() {
        InputOutput::OutputByte(CMOS_PORT, InputOutput::InputByte(CMOS_PORT) | NMI_DISABLE_FLAG);
    }

    inline bool NonMaskableInterruptsEnabled() {
        return (InputOutput::InputByte(CMOS_PORT) & NMI_DISABLE_FLAG) == 0;
    }
}  // namespace FunnyOS::HW

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_TCC
