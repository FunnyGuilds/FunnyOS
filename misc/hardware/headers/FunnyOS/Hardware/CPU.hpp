#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Stdlib/String.hpp>

namespace FunnyOS::HW::CPU {

    /**
     * Halts the CPU until an interrupt happens.
     * Can be used to hang the CPU indefinitely if InterruptFlag = 0.
     */
    inline void Halt();

    enum class Flags : uintmax_t {
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
        AlignmentCheck = 1 << 18,
        VirtualInterruptFlag = 1 << 19,
        VirtualInterruptPending = 1 << 20,
        CPUID_Supported = 1 << 21,
    };

    enum class CPUIDExtendedFeatures : uint64_t {
        /**
         * Onboard x87 FPU
         */
        FPU = (1ULL << 0),

        /**
         * Virtual mode extensions (VIF)
         */
        VME = (1ULL << 1),

        /**
         * Debugging extensions (CR4 bit 3)
         */
        DE = (1ULL << 2),

        /**
         * Page Size Extension
         */
        PSE = (1ULL << 3),

        /**
         * Time Stamp Counter
         */
        TSC = (1ULL << 4),

        /**
         * Model-specific registers
         */
        MSR = (1ULL << 5),

        /**
         * Physical Address Extension
         */
        PAE = (1ULL << 6),

        /**
         * Machine Check Exception
         */
        MCE = (1ULL << 7),

        /**
         * CMPXCHG8 (compare-and-swap) instruction
         */
        CX8 = (1ULL << 8),

        /**
         * Onboard Advanced Programmable Interrupt Controller
         */
        APIC = (1ULL << 9),

        /**
         * SYSCALL and SYSRET instructions
         */
        SYSCALL = (1ULL << 11),

        /**
         * Memory Type Range Registers
         */
        MTRR = (1ULL << 12),

        /**
         * Page Global Enable bit in CR4
         */
        PGE = (1ULL << 13),

        /**
         * Machine check architecture
         */
        MCA = (1ULL << 14),

        /**
         * Conditional move and FCMOV instructions
         */
        CMOV = (1ULL << 15),

        /**
         * Page Attribute Table
         */
        PAT = (1ULL << 16),

        /**
         * 36-bit page size extension
         */
        PSE36 = (1ULL << 17),

        /**
         * Multiprocessor Capable
         */
        MP = (1ULL << 19),

        /**
         * NX bit
         */
        NX = (1ULL << 20),

        /**
         * Extended MMX
         */
        MMXEXT = (1ULL << 22),

        /**
         * MMX instructions
         */
        MMX = (1ULL << 23),

        /**
         * FXSAVE, FXRSTOR instructions, CR4 bit 9
         */
        FXSR = (1ULL << 24),

        /**
         * FXSAVE/FXRSTOR optimizations
         */
        FXSR_OPT = (1ULL << 25),

        /**
         * Gibibyte pages
         */
        PDPE1GB = (1ULL << 26),

        /**
         * RDTSCP instruction
         */
        RDTSCP = (1ULL << 27),

        /**
         * Long mode
         */
        LM = (1ULL << 29),

        /**
         *  Extended 3DNow!
         */
        EXT_3DNOW = (1ULL << 30),

        /**
         * 3DNow!
         */
        HAS_3DNOW = (1ULL << 31),

        /**
         * LAHF / SAHF in long mode
         */
        LAHF_LM = (1ULL << 32),

        /**
         * Hyperthreading not valid
         */
        CMP_LEGACY = (1ULL << 33),

        /**
         * Secure virtual machine
         */
        SVM = (1ULL << 34),

        /**
         * Extended APIC space
         */
        EXTAPIC = (1ULL << 35),

        /**
         * CR8 in 32 -bit mode
         */
        CR8_LEGACY = (1ULL << 36),

        /**
         * Advanced bit manipulation(lzcnt and popcnt)
         */
        ABM = (1ULL << 37),

        /**
         * SSE4a
         */
        SSE4A = (1ULL << 38),

        /**
         * Misaligned SSE mode
         */
        MISALIGN_SSE = (1ULL << 39),

        /**
         * PREFETCH and PREFETCHW instructions
         */
        HAS_3DNOWPREFETCH = (1ULL << 40),

        /**
         * OS Visible Workaround
         */
        OSVW = (1ULL << 41),

        /**
         * Instruction Based Sampling
         */
        IBS = (1ULL << 42),

        /**
         * XOP instruction set
         */
        XOP = (1ULL << 43),

        /**
         * SKINIT / STGI instructions
         */

        SKINIT = (1ULL << 44),

        /**
         * Watchdog timer
         */
        WDT = (1ULL << 45),

        /**
         * Light Weight Profiling[22]
         */
        LWP = (1ULL << 47),

        /**
         * 4 operands fused multiply-add
         */
        FMA4 = (1ULL << 48),

        /**
         * Translation Cache Extension
         */
        TCE = (1ULL << 49),

        /**
         * NodeID MSR
         */
        NODEID_MSR = (1ULL << 51),

        /**
         * Trailing Bit Manipulation
         */
        TBM = (1ULL << 53),

        /**
         * Topology Extensions
         */
        TOPOEXT = (1ULL << 54),

        /**
         * Core performance counter extensions
         */
        PERFCTR_CORE = (1ULL << 55),

        /**
         * NB performance counter extensions
         */

        PERFCTR_NB = (1ULL << 56),

        /**
         * Data breakpoint extensions
         */
        DBX = (1ULL << 58),

        /**
         * Performance TSC
         */

        PERFTSC = (1ULL << 59),

        /**
         * L2I perf counter extensions
         */
        PCX_L2I = (1ULL << 60)
    };

    /**
     * Returns the contents of EFLAGS register (or RFLAGS in 64-bit mode)
     *
     * @return the contents of EFLAGS register (or RFLAGS in 64-bit mode)
     */
    inline uintmax_t GetFlagsRegister();

    /**
     * Checks whether or not the CPUID instruction is supported.
     *
     * @return whether or not the CPUID instruction is supported.
     */
    inline bool SupportsCpuid();

    /**
     * Gets the max non-extended feature number supported by CPUID.
     *
     * @return the max non-extended feature number supported by CPUID.
     */
    inline uint32_t GetCpuidMaxFeature();

    /**
     * Gets the max extended feature number supported by CPUID.
     *
     * @return the max extended feature number supported by CPUID.
     */
    inline uint32_t GetCpuidMaxExtendedFeature();

    /**
     * Calls CPUId instruction.
     *
     * @param eax contents of EAX register
     * @param ecx contents of ECX register
     * @param edx contents of EDX register
     * @param ebx contents of EBX register
     */
    inline void CallCpuid(uint32_t& eax, uint32_t& ecx, uint32_t& edx, uint32_t& ebx);

    /**
     * Retrieves vendor id string from CPUID.
     *
     * @param buffer buffer to hold the string, must be at least 13 bytes long.
     */
    inline void GetVendorId(Stdlib::String::StringBuffer& buffer);

    /**
     * Get extended feature bits.
     *
     * @return extended feature bits
     */
    inline uint64_t GetExtendedFeatureBits();

    /**
     * Retrieves processor brand string from CPUID.
     *
     * @param buffer buffer to hold the string, must be at least 48 bytes long.
     * @return true if success, false if the process brand string feature is not supported or the buffer is too small.
     */
    inline bool GetBrandString(Stdlib::String::StringBuffer& buffer);

}  // namespace FunnyOS::HW::CPU

#ifdef __GNUC__
#include "CPU_GNUC.tcc"
#endif
#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP
