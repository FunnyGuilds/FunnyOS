#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP
#error "Include CPU.hpp instead"
#endif
#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_GNUC_TCC
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_GNUC_TCC

namespace FunnyOS::HW::CPU {
    inline void Halt() {
        asm volatile("hlt");
    }

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
