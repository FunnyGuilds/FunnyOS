#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_BIOS_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_BIOS_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::HW::BIOS {

    /**
     * Gets a segment part of a real mode [address]
     */
    uint16_t GetRealModeSegment(void* address);

    /**
     * Gets an offset part of a real mode [address]
     */
    uint16_t GetRealModeOffset(void* address);

    /**
     * Setups the CallBios interface.
     *
     * All parameters are indexes to GDT selectors, where 0 = null descriptor, 1 = first descriptor, etc...
     *
     * @param selector64BitCode index of a 64-bit ring 0 code selector in GDT
     * @param selector64BitData index of a 64-bit data selector in GDT
     * @param selector16BitCode index of a 16-bit ring 0 code selector in GDT
     * @param selector16BitData index of a 16-bit data selector in GDT
     */
    void SetupCallBiosInterface(
        uint16_t selector64BitCode, uint16_t selector64BitData, uint16_t selector16BitCode, uint16_t selector16BitData);

    /**
     * Calls a BIOS interrupt.
     *
     * @param interrupt interrupt number
     * @param format input and output registers.
     *      For example: "eax, bh, es:di, =ecx" - eax and bh, es:di are input registers, ecx is an output register.
     * @param ... parameters, in order according to format.
     *  Parameters for:
     *      Input registers: (e.g. dl, dx, edx) must be of type uint8_t, uint16_t, uint32_t depending on register type.
     *      Input segment:offset register pairs (e.g. es:di, fs:dx) must be of any pointer type
     *      Output registers (e.g. =ah, =di): must be of type uint8_t*, uint16_t*, uint32_t* depending on register type.
     *
     * @return true if the CF flags in EFLAGS register is clear
     */
    bool CallBios(uint8_t interrupt, const char* format, ...);

}  // namespace FunnyOS::HW::BIOS

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_BIOS_HPP
