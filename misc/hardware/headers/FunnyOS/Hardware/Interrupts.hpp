#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Stdlib/Functional.hpp>

namespace FunnyOS::HW {
    /**
     * Number of possible interrupt types.
     */
    constexpr const size_t INTERRUPTS_COUNT = 256;

    /**
     * Represents type of an interrupt.
     */
    enum class InterruptType : uintmax_t {
        DivideError = 0,
        DebugException = 1,
        NmiInterrupt = 2,
        Breakpoint = 3,
        Overflow = 4,
        BoundRangeExceeded = 5,
        InvalidOpcode = 6,
        DeviceNotAvailable = 7,
        DoubleFault = 8,
        CoprocessorSegmentOverrun = 9,
        InvalidTSS = 10,
        SegmentNotPresent = 11,
        StackSegmentFault = 12,
        GeneralProtection = 13,
        PageFault = 14,
        Reserved15 = 15,
        x87FPU_FloatingPointError = 16,
        AlignmentCheck = 17,
        MachineCheck = 18,
        SIMD_FloatingPointException = 19,
        VirtualizationException = 20,
        Reserved21 = 21,
        Reserved22 = 22,
        Reserved23 = 23,
        Reserved24 = 24,
        Reserved25 = 25,
        Reserved26 = 26,
        Reserved27 = 27,
        Reserved28 = 28,
        Reserved29 = 29,
        Reserved30 = 30,
        Reserved31 = 31,
        IRQ_PIT_Interrupt = 32,
        IRQ_KeyboardInterrupt = 33,
        IRQ_CascadeInterrupt = 34,
        IRQ_COM2_Interrupt = 35,
        IRQ_COM1_Interrupt = 36,
        IRQ_LPT2_Interrupt = 36,
        IRQ_FloppyInterrupt = 37,
        IRQ_LPT1_Interrupt = 38,  // TODO: Handle spurious
        IRQ_CMOS_RealTimeClockInterrupt = 39,
        IRQ_ACPI_Interrupt = 40,
        IRQ_10_Interrupt = 41,
        IRQ_11_Interrupt = 42,
        IRQ_PS2_MouseInterrupt = 43,
        IRQ_CoProcessor_FPU_IPI_Interrupt = 44,
        IRQ_PrimaryAtaHardDriveInterrupt = 45,
        IRQ_SecondaryAtaHardDriveInterrupt = 46,
    };

    Stdlib::Optional<const char*> GetInterruptMnemonic(InterruptType type);

    /**
     * Size of a platform register.
     */
    using Register = uintmax_t;

    /**
     * Interrupt data
     */
    struct InterruptData {
#ifdef F_64
        // TODO
#else
        /**
         * Preserved value of the EAX register.
         */
        Register EAX;

        /**
         * Preserved value of the ECX register.
         */
        Register ECX;

        /**
         * Preserved value of the EDX register.
         */
        Register EDX;

        /**
         * Preserved value of the EBX register.
         */
        Register EBX;

        /**
         * Preserved value of the ESP register.
         */
        Register ESP;

        /**
         * Preserved value of the EBP register.
         */
        Register EBP;

        /**
         * Preserved value of the ESI register.
         */
        Register ESI;

        /**
         * Preserved value of the EDI register.
         */
        Register EDI;
#endif

        /**
         * Type of the interrupt.
         */
        const InterruptType Type;

        /**
         * Error code if any.
         * Only for selected CPU interrupts, 0 in any other case.
         */
        const uintmax_t ErrorCode;

#ifdef F_64
        /**
         * Preserved value of the RIP register.
         */
        Register RIP;

        /**
         * Preserved value of the CS register.
         */
        Register CS;

        /**
         * Preserved value of the RFLAGS register.
         */
        Register RFLAGS;
#else
        /**
         * Preserved value of the EIP register.
         */
        Register EIP;

        /**
         * Preserved value of the CS register.
         */
        Register CS;

        /**
         * Preserved value of the EFLAGS register.
         */
        Register EFLAGS;
#endif
    };

    /**
     * An interrupt handler routine.
     */
    using InterruptHandler = void (*)(InterruptData* data);

    void RegisterInterruptHandler(InterruptType type, InterruptHandler handler);

    void UnregisterInterruptHandler(InterruptType type);

    void RegisterUnknownInterruptHandler(InterruptHandler handler);

    void SetupInterrupts();

    F_ALWAYS_INLINE inline void EnableHardwareInterrupts();

    F_ALWAYS_INLINE inline void DisableHardwareInterrupts();

    F_ALWAYS_INLINE inline bool HardwareInterruptsEnabled();

    F_ALWAYS_INLINE inline void EnableNonMaskableInterrupts();

    F_ALWAYS_INLINE inline void DisableNonMaskableInterrupts();

    F_ALWAYS_INLINE inline bool NonMaskableInterruptsEnabled();

    class NoInterruptsBlock {
       public:
        NON_MOVEABLE(NoInterruptsBlock);
        NON_COPYABLE(NoInterruptsBlock);

        NoInterruptsBlock();

        ~NoInterruptsBlock();

       private:
        bool m_hadInterrupts;
        bool m_hadNMIs;
    };

}  // namespace FunnyOS::HW

#include "Interrupts.tcc"
#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_HPP
