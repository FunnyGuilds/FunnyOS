#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_IDT_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_IDT_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::Bootloader32::IDT {
    // TODO

    struct IDTR {
        uint16_t Length;
        uint32_t Offset;
    };

    struct IDTDescriptor {
        uint16_t OffsetLow;
        uint16_t Selector;
        uint8_t Reserved;
        uint8_t TypeAttribute;
        uint16_t OffsetHigh;
    };

}  // namespace FunnyOS::Bootloader32::IDT


#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_IDT_HPP
