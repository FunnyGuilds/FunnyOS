#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::HW::CPU {

    /**
     * Halts the CPU until an interrupt happens.
     * Can be used to hang the CPU indefinitely if InterruptFlag = 0.
     */
    inline void Halt();

    enum Flags : uintmax_t {
        CarryFlag = 1 << 0,
        ParityFlag = 1 << 2,
        AdjustFlag = 1 << 4,
        ZeroFlag = 1 << 6,
        SignFlag = 1 << 7,
        TrapFlag = 1 << 8,
        InterruptFlag = 1 << 9,
        DirectionFlag = 1 << 10,
        OverflowFlag = 1 << 11,
        IOPL_Bit0 = 1 << 12,
        IOPL_Bit2 = 1 << 13,
        NestedTaskFlag = 1 << 14,
        ResumeFlag = 1 << 16,
        Virtual8086_ModeFlag = 1 << 17,
        AlingmentCheck = 1 << 18,
        VirtualInterruptFlag = 1 << 19,
        VirtualInterruptPending = 1 << 20,
        CPUID_Supported = 1 << 21,
    };

    /**
     * Returns the contents of EFLAGS register (or RFLAGS in 64-bit mode)
     *
     * @return the contents of EFLAGS register (or RFLAGS in 64-bit mode)
     */
    inline uintmax_t GetFlagsRegister();

}

#ifdef __GNUC__
#   include "CPU_GNUC.tcc"
#endif
#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP
