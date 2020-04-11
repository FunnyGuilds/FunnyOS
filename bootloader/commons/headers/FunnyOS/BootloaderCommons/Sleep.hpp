#ifndef FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_SLEEP_HPP
#define FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_SLEEP_HPP

#include <FunnyOS/Hardware/Interrupts.hpp>

namespace FunnyOS::Bootloader {

    /**
     * Setups the Programmable Interval Timer Chipset to produce the IRQ0 every 1 ms.
     * Must be called for the Sleep method to work.
     */
    void SetupPIT();

    /**
     * Sleeps for the given amount of milliseconds.
     */
    void Sleep(unsigned int milliseconds);

    /**
     * Handler for IRQ0 interrupt.
     * Must be set for the Sleep method to work.
     */
    void PITInterruptHandler(HW::InterruptData* data);

}  // namespace FunnyOS::Bootloader

#endif  // FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_SLEEP_HPP
