#include "A20Line.hpp"

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Hardware/InputOutput.hpp>

namespace FunnyOS::Bootloader32::A20 {
    using namespace HW::InputOutput;

    /** Port number for FastA20 gate */
    constexpr const uint16_t PORT_FAST2A20 = 0x92;

    bool IsEnabled() {
        volatile auto *address1 = reinterpret_cast<uint8_t *>(0x0007FFFF);
        volatile auto *address2 = reinterpret_cast<uint8_t *>(0x0017FFFF);

        // These memory locations should be unused but who knows.
        uint8_t v1 = *address1;
        uint8_t v2 = *address2;

        // Check
        *address1 = 'S';
        *address2 = 'D';
        bool hasA20 = *address1 != *address2;

        // Restore values
        *address1 = v1;
        *address2 = v2;

        return hasA20;
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