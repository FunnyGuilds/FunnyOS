#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_BIOS_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_BIOS_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::Bootloader64::BIOS {

    uint16_t GetRealModeSegment(void* address);

    uint16_t GetRealModeOffset(void* address);

    void CallBios(uint8_t interrupt, const char* format, ...);

}

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_BIOS_HPP
