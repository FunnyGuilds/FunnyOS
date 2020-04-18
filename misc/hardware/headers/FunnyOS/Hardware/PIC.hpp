#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_PIC_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_PIC_HPP

#include "Interrupts.hpp"

/**
 * The PIC 8259 communication layer.
 */
namespace FunnyOS::HW::PIC {

    /**
     * Checks whether or not the specified interrupt type is generated by PIC .
     *
     * @param type to check
     * @return whether or not the specified interrupt type is generated by PIC
     */
    bool IsPICInterrupt(InterruptType type);

    /**
     * Checks whether or not the specified interrupt type is generated by the master PIC .
     *
     * @param type to check
     * @return whether or not the specified interrupt type is generated by the master PIC
     */
    bool IsMasterPICInterrupt(InterruptType type);

    /**
     * Remaps the Programmable Interrupt Controller to assign proper interrupt values.
     */
    void Remap();

    /**
     * Send the end of interrupt command to one of the PIC controllers, depending on parameter.
     *
     * @param master if true the end of interrupt will be send to master PIC, if false it will be send to slave PIC
     */
    void SendEndOfInterrupt(bool master);
}  // namespace FunnyOS::HW::PIC

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_PIC_HPP