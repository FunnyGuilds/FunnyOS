#ifndef FUNNYOS_MISC_HARDWARE_SRC_INTERRUPTSETUP_INTERRUPTSETUP_HPP
#define FUNNYOS_MISC_HARDWARE_SRC_INTERRUPTSETUP_INTERRUPTSETUP_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Hardware/Interrupts.hpp>

namespace FunnyOS::HW::InterruptSetup {

    /**
     * Interrupt routine address, matching the structure in interrupt_setup.asm
     */
    struct InterruptRoutinesData {
        uint32_t FirstRoutineAddress;
        uint8_t InterruptRoutineSize;
    } F_DONT_ALIGN;

    /**
     * Gets the generated InterruptRoutinesData.
     */
    const InterruptRoutinesData& GetInterruptRoutinesData();

#ifndef F_64
    /**
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
    struct IDT {
        uint16_t Limit;
        uint32_t BaseAddress;
    } F_DONT_ALIGN;
#endif

    /**
     * Location of a segment selector.
     */
    enum class SegmentSelectorLocation {
        /**
         * Means that the segment is located in global descriptor table.
         */
        GDT = 0,

        /**
         * Means that the segment is located in local descriptor table.
         **/
        LDT = 1
    };

    /**
     * Type of a segment selector.
     */
    using SegmentSelector = uint16_t;

    /**
     * Create a segment selector.
     *
     * @param rpl requested privilege level
     * @param location whether the segment is located in GDT or LDT.
     * @param index number of the selector (when 1 is the first selector, 0 is reserved for the null selector)
     * @return
     */
    uint16_t CreateSegmentSelector(uint8_t rpl, SegmentSelectorLocation location, uint16_t index);

    /**
     * Sets up the entire interrupt table and activates it.
     */
    void SetupInterruptTable(InterruptHandler handler);
}  // namespace FunnyOS::HW::InterruptSetup

#endif  // FUNNYOS_MISC_HARDWARE_SRC_INTERRUPTSETUP_INTERRUPTSETUP_HPP
