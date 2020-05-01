#ifndef FUNNYOS_MISC_HARDWARE_SRC_INTERRUPTSETUP_INTERRUPTSETUP_HPP
#define FUNNYOS_MISC_HARDWARE_SRC_INTERRUPTSETUP_INTERRUPTSETUP_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include "Interrupts.hpp"

namespace FunnyOS::Bootloader32 {

    /*
     * IDT 32-bit Interrupt Gate Descriptor.
     */
    struct InterruptGateDescriptor {
        uint16_t OffsetLow;
        uint16_t SegmentSelector;
        uint8_t Reserved;
        uint8_t Flags;
        uint16_t OffsetHigh;
    } F_DONT_ALIGN;

    /**
     * 32-bit structure of the IDTR register.
     */
    struct IDTR {
        uint16_t Limit;
        uint32_t BaseAddress;
    } F_DONT_ALIGN;

    /**
     * Sets up the entire interrupt table and activates it.
     */
    void SetupInterruptTable(InterruptHandler handler);
}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_MISC_HARDWARE_SRC_INTERRUPTSETUP_INTERRUPTSETUP_HPP
