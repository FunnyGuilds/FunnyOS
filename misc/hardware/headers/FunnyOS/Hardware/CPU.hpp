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
        CarryFlag               = 1 << 0,
        ParityFlag              = 1 << 2,
        AdjustFlag              = 1 << 4,
        ZeroFlag                = 1 << 6,
        SignFlag                = 1 << 7,
        TrapFlag                = 1 << 8,
        InterruptFlag           = 1 << 9,
        DirectionFlag           = 1 << 10,
        OverflowFlag            = 1 << 11,
        IOPL_Bit0               = 1 << 12,
        IOPL_Bit2               = 1 << 13,
        NestedTaskFlag          = 1 << 14,
        ResumeFlag              = 1 << 16,
        Virtual8086_ModeFlag    = 1 << 17,
        AlignmentCheck          = 1 << 18,
        VirtualInterruptFlag    = 1 << 19,
        VirtualInterruptPending = 1 << 20,
        CPUID_Supported         = 1 << 21,
    };

    enum class CPUIDFeatures : uint64_t {
        /**
         * Onboard x87 FPU
         */
        FPU = (1ULL << 0),

        /**
         * Virtual 8086 mode extensions (such as VIF, VIP, PIV)
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
         * SYSENTER and SYSEXIT instructions
         */
        SEP = (1ULL << 11),

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
         * Processor Serial Number
         */
        PSN = (1ULL << 18),

        /**
         * CLFLUSH instruction (SSE2)
         */
        CLFSH = (1ULL << 19),

        /**
         * Debug store: save trace of executed jumps
         */
        DS = (1ULL << 21),

        /**
         * Onboard thermal control MSRs for ACPI
         */
        ACPI = (1ULL << 22),

        /**
         * MMX instructions	popcnt
         */
        MMX = (1ULL << 23),

        /**
         * FXSAVE, FXRESTOR instructions, CR4 bit 9
         */
        FXSR = (1ULL << 24),

        /**
         * SSE instructions (a.k.a. Katmai New Instructions)
         */
        SSE = (1ULL << 25),

        /**
         * SSE2 instructions
         */
        SSE2 = (1ULL << 26),

        /**
         * CPU cache implements self-snoop
         */
        SS = (1ULL << 27),

        /**
         * Hyper-threading
         */
        HTT = (1ULL << 28),

        /**
         * Thermal monitor automatically limits temperature
         */
        TM = (1ULL << 29),

        /**
         * IA64 processor emulating x86
         */
        IA64 = (1ULL << 30),

        /**
         * Pending Break Enable (PBE# pin) wakeup capability
         */
        PBE = (1ULL << 31),

        /**
         * Prescott New Instructions-SSE3 (PNI)
         */
        SSE3 = (1ULL << 32),

        /**
         * PCLMULQDQ
         */
        PCLMULQDQ = (1ULL << 33),

        /**
         * 64-bit debug store (edx bit 21)
         */
        DTES64 = (1ULL << 34),

        /**
         * MONITOR and MWAIT instructions (SSE3)
         */
        MONITOR = (1ULL << 35),

        /**
         * CPL qualified debug store
         */
        DSCPL = (1ULL << 36),

        /**
         * Virtual Machine eXtensions
         */
        VMX = (1ULL << 37),

        /**
         * Safer Mode Extensions (LaGrande)
         */
        SMX = (1ULL << 38),

        /**
         * Enhanced SpeedStep
         */
        EST = (1ULL << 39),

        /**
         * Thermal Monitor 2
         */
        TM2 = (1ULL << 40),

        /**
         * Supplemental SSE3 instructions
         */
        SSSE3 = (1ULL << 41),

        /**
         * L1 Context ID
         */
        CNXTID = (1ULL << 42),

        /**
         * Silicon Debug interface
         */
        SDBG = (1ULL << 43),

        /**
         * Fused multiply-add (FMA3)
         */
        FMA = (1ULL << 44),

        /**
         * CMPXCHG16B instruction
         */
        CX16 = (1ULL << 45),

        /**
         * Can disable sending task priority messages
         */
        XTPR = (1ULL << 46),

        /**
         * Perfmon & debug capability
         */
        PDCM = (1ULL << 47),

        /**
         * Process context identifiers (CR4 bit 17)
         */
        PCID = (1ULL << 49),

        /**
         * Direct cache access for DMA writes[7][8]
         */
        DCA = (1ULL << 50),

        /**
         * SSE4.1 instructions
         */
        SSE41 = (1ULL << 51),

        /**
         * SSE4.2 instructions
         */
        SSE42 = (1ULL << 52),

        /**
         * x2APIC
         */
        X2APIC = (1ULL << 53),

        /**
         * MOVBE instruction (big-endian)
         */
        MOVBE = (1ULL << 54),

        /**
         * instruction
         */
        POPCNT = (1ULL << 55),

        /**
         * APIC implements one-shot operation using a TSC deadline value
         */
        TSCDEADLINE = (1ULL << 56),

        /**
         * AES instruction set
         */
        AES = (1ULL << 57),

        /**
         * XSAVE, XRESTOR, XSETBV, XGETBV
         */
        XSAVE = (1ULL << 58),

        /**
         * XSAVE enabled by OS
         */
        OSXSAVE = (1ULL << 59),

        /**
         * Advanced Vector Extensions
         */
        AVX = (1ULL << 60),

        /**
         * F16C (half-precision) FP feature
         */
        F16C = (1ULL << 61),

        /**
         * RDRAND (on-chip random number generator) feature
         */
        RDRND = (1ULL << 62),

        /**
         * Hypervisor present (always zero on physical CPUs)[9][10]
         */
        HYPERVISOR = (1ULL << 63),
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
     * Commons MSRs
     */
    namespace MSR {
        /**
         * Extended Feature Enable Register (EFER)
         */
        constexpr const uint32_t EFER = 0xC0000080;
    }  // namespace MSR

    enum class EferBits : uint64_t {
        /**
         * System Call Extensions
         */
        SCE = (1 << 0),

        /**
         * AMD K6 only: Data Prefetch Enable
         */
        DPE = (1 << 1),

        /**
         * AMD K6 only: Speculative EWBE# Disable
         */
        SEWBED = (1 << 2),

        /**
         * AMD K6 only: Global EWBE# Disable
         */
        GEWBED = (1 << 3),

        /**
         * AMD K6 only: L2 Cache Disable
         */
        L2D = (1 << 4),

        /**
         * Long Mode Enable
         */
        LME = (1 << 8),

        /**
         * Long Mode Active
         */
        LMA = (1 << 10),

        /**
         * No-Execute Enable
         */
        NXE = (1 << 11),

        /**
         * Secure Virtual Machine Enable
         */
        SVME = (1 << 12),

        /**
         * Long Mode Segment Limit Enable
         */
        LMSLE = (1 << 13),

        /**
         * Fast FXSAVE/FXRSTOR
         */
        FFXSR = (1 << 14),

        /**
         * Translation Cache Extension
         */
        TCE = (1 << 15),
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
    inline uint64_t GetFeatureBits();

    /**
     * Decode all CPUID features into a string.
     *
     * @param feature feature bits (see GetFeatureBits())
     * @param buffer output string buffer
     * @return true on success, false if the buffer was too small
     */
    inline bool DecodeFeatureBits(uint64_t features, Stdlib::String::StringBuffer& buffer);

    /**
     * Get extended feature bits.
     *
     * @return extended feature bits
     */
    inline uint64_t GetExtendedFeatureBits();

    /**
     * Decode all CPUID extended features into a string.
     *
     * @param feature feature bits (see GetExtendedFeatureBits())
     * @param buffer output string buffer
     * @return true on success, false if the buffer was too small
     */
    inline bool DecodeExtendedFeatureBits(uint64_t features, Stdlib::String::StringBuffer& buffer);

    /**
     * Retrieves processor brand string from CPUID.
     *
     * @param buffer buffer to hold the string, must be at least 48 bytes long.
     * @return true if success, false if the process brand string feature is not supported or the buffer is too small.
     */
    inline bool GetBrandString(Stdlib::String::StringBuffer& buffer);

    /**
     * Read from a model specific register specified by [msr]
     *
     * @param msr the msr number
     * @return value of the msr
     */
    inline uint64_t ReadMSR(uint32_t msr);

    /**
     * Write from a model specific register specified by [msr]
     *
     * @param msr the msr number
     * @param value value to write to that msr
     */
    inline void WriteMSR(uint32_t msr, uint64_t value);

}  // namespace FunnyOS::HW::CPU

#ifdef __GNUC__
#include "CPU_GNUC.tcc"
#endif

#include "CPU.tcc"
#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP
