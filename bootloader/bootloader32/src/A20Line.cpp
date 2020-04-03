#include "A20Line.hpp"

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/BootloaderCommons/InputOutput.hpp>

namespace FunnyOS::Bootloader32::A20 {
    using namespace Bootloader::InputOutput;

    /** Port number for FastA20 gate */
    constexpr const uint16_t PORT_FAST2A20 = 0x92;

    bool IsEnabled() {
        // TODO
        return true;
    }

    void EnableA20WithFastA20() {
        uint8_t value = InputByte(PORT_FAST2A20);
        if ((value & 0x02U) != 0) {
            return;
        }

        value = static_cast<uint8_t>((value | 0x02U) & 0xFEU);
        OutputByte(PORT_FAST2A20, value);
    }

    void EnableA20Traditionally() {
        // TODO: implement me pls
    }

}  // namespace FunnyOS::Bootloader32::A20