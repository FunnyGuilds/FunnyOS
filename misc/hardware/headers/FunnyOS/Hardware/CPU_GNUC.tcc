#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP
#error "Include CPU.hpp instead"
#endif
#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_GNUC_TCC
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_GNUC_TCC

namespace FunnyOS::HW::CPU {
    namespace _CPUID {
        inline bool FetchSupportCpuid() {
            uint32_t origEflags = 0;
            uint32_t newEflags = 0;

            // CPUID is supported if the flag can be flipped.
            asm volatile(
                "pushfl\n"
                "pushfl\n"
                "pop %0\n"
                "xor %2, (%%esp)\n"
                "popfl\n"
                "pushfl\n"
                "pop %1\n"
                : "=a"(origEflags), "=c"(newEflags)
                : "d"(Flags::CPUID_Supported));

            return origEflags != newEflags;
        }
    }  // namespace _CPUID

    inline void Halt() {
        asm volatile("hlt");
    }

    inline uintmax_t GetFlagsRegister() {
        uintmax_t flags;
#ifdef F_64
        asm volatile(
            "pushfq\n"
            "pop %%rax"
            : "=a"(flags));
#else
        asm volatile(
            "pushfl\n"
            "pop %%eax"
            : "=a"(flags));
#endif
        return flags;
    }

    inline void CallCpuid(uint32_t& eax, uint32_t& ecx, uint32_t& edx, uint32_t& ebx) {
        asm("cpuid" : "=b"(ebx), "=c"(ecx), "=d"(edx), "=a"(eax) : "b"(ebx), "c"(ecx), "d"(edx), "a"(eax));
    }

    inline uint64_t ReadMSR(uint32_t msr) {
        uintmax_t low;
        uintmax_t high;

        asm("rdmsr" : "=a"(low), "=d"(high) : "c"(static_cast<uintmax_t>(msr)));

        return static_cast<uint64_t>(low & 0xFFFFFFFF) | static_cast<uint64_t>(high & 0xFFFFFFFF) << 32ULL;
    }

    inline void WriteMSR(uint32_t msr, uint64_t value) {
        asm("wrmsr"
            :
            : "a"(static_cast<uintmax_t>(value & 0xFFFFFFFF)), "d"(static_cast<uintmax_t>(value >> 32ULL)),
              "c"(static_cast<uintmax_t>(msr)));
    }
}  // namespace FunnyOS::HW::CPU

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_GNUC_TCC
