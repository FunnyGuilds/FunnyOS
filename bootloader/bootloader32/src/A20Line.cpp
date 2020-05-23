#include "A20Line.hpp"

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Hardware/InputOutput.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include "Logging.hpp"
#include "Sleep.hpp"

namespace FunnyOS::Bootloader32::A20 {
    using namespace HW::InputOutput;

    /** Port number for FastA20 gate */
    constexpr const uint16_t PORT_FAST2A20 = 0x92;

    bool IsEnabled() {
        volatile auto* address1 = reinterpret_cast<uint8_t*>(0x0007FFFF);
        volatile auto* address2 = reinterpret_cast<uint8_t*>(0x0017FFFF);

        // These memory locations should be unused but who knows.
        uint8_t v1 = *address1;
        uint8_t v2 = *address2;

        // Check
        *address1 = 'S';
        *address2 = 'D';

        const bool hasA20 = *address1 != *address2;

        // Restore values
        *address1 = v1;
        *address2 = v2;

        return hasA20;
    }

    void EnableA20WithFastA20() {
        HW::NoInterruptsBlock noInterrupts;

        uint8_t value = InputByte(PORT_FAST2A20);
        if ((value & 0x02U) != 0) {
            return;
        }

        value = static_cast<uint8_t>((value | 0x02U) & 0xFEU);
        OutputByte(PORT_FAST2A20, value);
    }

    void EnableA20ViaEEPort() {
        HW::InputOutput::InputByte(0xEE);
    }

    void EnableA20Traditionally() {
        HW::PS2::EnableA20();
    }

    void TryEnable() {
        for (size_t i = 0; i < 3; i++) {
            EnableA20Traditionally();
            Sleep(100);

            FB_LOG_DEBUG_F("Trying to enable A20 via keyboard controller... (Try %d/3)", i + 1);

            if (IsEnabled()) {
                return;
            }

            FB_LOG_DEBUG("...Failed");
        }

        FB_LOG_DEBUG("Using the fast A20 gate");
        EnableA20WithFastA20();
        Sleep(100);

        if (IsEnabled()) {
            return;
        }
        FB_LOG_DEBUG("... Failed");

        FB_LOG_DEBUG("Using the EE port");
        EnableA20ViaEEPort();
        Sleep(100);

        if (IsEnabled()) {
            return;
        }
        FB_LOG_DEBUG("... Failed");
    }
}  // namespace FunnyOS::Bootloader32::A20