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
    enum class InterruptType : uint64_t {
        DivideError                        = 0,
        DebugException                     = 1,
        NmiInterrupt                       = 2,
        Breakpoint                         = 3,
        Overflow                           = 4,
        BoundRangeExceeded                 = 5,
        InvalidOpcode                      = 6,
        DeviceNotAvailable                 = 7,
        DoubleFault                        = 8,
        CoprocessorSegmentOverrun          = 9,
        InvalidTSS                         = 10,
        SegmentNotPresent                  = 11,
        StackSegmentFault                  = 12,
        GeneralProtection                  = 13,
        PageFault                          = 14,
        Reserved15                         = 15,
        x87FPU_FloatingPointError          = 16,
        AlignmentCheck                     = 17,
        MachineCheck                       = 18,
        SIMD_FloatingPointException        = 19,
        VirtualizationException            = 20,
        Reserved21                         = 21,
        Reserved22                         = 22,
        Reserved23                         = 23,
        Reserved24                         = 24,
        Reserved25                         = 25,
        Reserved26                         = 26,
        Reserved27                         = 27,
        Reserved28                         = 28,
        Reserved29                         = 29,
        Reserved30                         = 30,
        Reserved31                         = 31,
        IRQ_PIT_Interrupt                  = 32,
        IRQ_KeyboardInterrupt              = 33,
        IRQ_CascadeInterrupt               = 34,
        IRQ_COM2_Interrupt                 = 35,
        IRQ_COM1_Interrupt                 = 36,
        IRQ_LPT2_Interrupt                 = 36,
        IRQ_FloppyInterrupt                = 37,
        IRQ_LPT1_Interrupt                 = 38,  // TODO: Handle spurious
        IRQ_CMOS_RealTimeClockInterrupt    = 39,
        IRQ_ACPI_Interrupt                 = 40,
        IRQ_10_Interrupt                   = 41,
        IRQ_11_Interrupt                   = 42,
        IRQ_PS2_MouseInterrupt             = 43,
        IRQ_CoProcessor_FPU_IPI_Interrupt  = 44,
        IRQ_PrimaryAtaHardDriveInterrupt   = 45,
        IRQ_SecondaryAtaHardDriveInterrupt = 46,
    };

    Stdlib::Optional<const char*> GetInterruptMnemonic(InterruptType type);

    /**
     * Sets the IF flag in FLAGS register.
     *
     * If this flag is set the maskable hardware interrupts will be handled.
     */
    F_ALWAYS_INLINE inline void EnableHardwareInterrupts();

    /**
     * Clears the IF flag in FLAGS register.
     *
     * If this flag is cleared the maskable hardware interrupts will be ignored.
     */
    F_ALWAYS_INLINE inline void DisableHardwareInterrupts();

    /**
     * Retruns whether or not the IF flag in FLAGS register is set.
     *
     * @return whether or not the IF flag in FLAGS register is set.
     */
    F_ALWAYS_INLINE inline bool HardwareInterruptsEnabled();

    /**
     * Enables the non-maskable interrupt handling by clearing the NMI disable flag in CMOS.
     *
     * If this flag is cleared the CPU wil generate int 2 when a non-maskable interrupt happen.
     */
    F_ALWAYS_INLINE inline void EnableNonMaskableInterrupts();

    /**
     * Disables the non-maskable interrupt handling by setting the NMI disable flag in CMOS.
     *
     * If this flag is set the CPU will ignore non-maskable interrupt.
     * The NMIs should never be disabled for an extended periods of time and should be always properly handled.
     */
    F_ALWAYS_INLINE inline void DisableNonMaskableInterrupts();

    /**
     * Checks whether or not the CPU handles non-maskable interrupts.
     *
     * @return  whether or not the CPU handles non-maskable interrupts
     */
    F_ALWAYS_INLINE inline bool NonMaskableInterruptsEnabled();

    /**
     * RAII class for disabling interrupts for specific blocks.
     */
    class NoInterruptsBlock {
       public:
        NON_MOVEABLE(NoInterruptsBlock);
        NON_COPYABLE(NoInterruptsBlock);

        /**
         * Disables the interrupts (including NMIs) if they are enabled.
         */
        NoInterruptsBlock();

        /**
         * Enables the interrupts (including NMIs) if they were enabled when the constructor was called.
         */
        ~NoInterruptsBlock();

       private:
        bool m_hadInterrupts;
        bool m_hadNMIs;
        uint16_t m_picMask;
    };

}  // namespace FunnyOS::HW

#include "Interrupts.tcc"
#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INTERRUPTS_HPP
