#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP
#error "Include CPU.hpp instead"
#endif
#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_GNUC_TCC
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_GNUC_TCC

namespace FunnyOS::HW::CPU {
    inline void Halt() {
        asm volatile("hlt");
    }

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

    inline uintmax_t GetFlagsRegister() {
        uintmax_t flags;
#ifdef F_64
        asm volatile(
            "pushfq\n"
            "pop rax"
            : "=a"(flags));
#else
        asm volatile(
            "pushfd\n"
            "pop eax"
            : "=a"(flags));
#endif
        return flags;
    }
}  // namespace FunnyOS::HW::CPU

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_GNUC_TCC
