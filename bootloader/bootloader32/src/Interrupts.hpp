#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_INTERRUPTS_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_INTERRUPTS_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Hardware/Interrupts.hpp>

namespace FunnyOS::Bootloader32 {

    /**
     * Setups all the interrupt handlers, loads IDT, reprograms PIC and enables all interrupts.
     */
    void SetupInterrupts();

    /**
     * Pause until enter is pressed.
     */
    void Pause();

    /**
     * Size of a platform register.
     */
    using Register = uint32_t;

    /**
     * Interrupt data
     */
    struct InterruptData {
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

        /**
         * Type of the interrupt.
         */
        const HW::InterruptType Type;

        /**
         * Error code if any.
         *
         * Only for selected CPU interrupts, 0 in any other case.
         */
        const uintmax_t ErrorCode;

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
    };

    /**
     * An interrupt handler routine.
     */
    using InterruptHandler = void (*)(InterruptData* data);

    /**
     * Sets a handler for the specified interrupt type.
     * It will replace the previous handler.
     *
     * @param type type of the interrupt.
     * @param handler new handler
     */
    void RegisterInterruptHandler(HW::InterruptType type, InterruptHandler handler);

    /**
     * Removes a handler for the specified interrupt type making the interrupt being handled by the Unknown Interrupt
     * Handler.
     *
     * @param type type of the interrupt.
     */
    void UnregisterInterruptHandler(HW::InterruptType type);

    /**
     * Registers a handler for all interrupts that do not have their own handlers registered.
     * It will replace the previous handler.
     *
     * @param handler new handler
     */
    void RegisterUnknownInterruptHandler(InterruptHandler handler);

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_INTERRUPTS_HPP
