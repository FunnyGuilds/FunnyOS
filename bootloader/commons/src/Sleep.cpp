#include <FunnyOS/BootloaderCommons/Sleep.hpp>
#include <FunnyOS/BootloaderCommons/Logging.hpp>

#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Hardware/InputOutput.hpp>

namespace FunnyOS::Bootloader {
    unsigned int sleepTimer;

    void SetupPIT() {
        using namespace HW::InputOutput;

        constexpr static const auto divider = static_cast<uint16_t>(1193182.0 / 1000.0);

        HW::NoInterruptsBlock noInterrupts;
        OutputByte(0x43, 0b00110100);
        OutputByte(0x40, divider & 0xFF);
        OutputByte(0x40, (divider >> 8) & 0xFF);
    }

    void Sleep(unsigned int milliseconds) {
        sleepTimer = milliseconds;
        while (sleepTimer > 0) {
            HW::CPU::Halt();
        }
    }

    void PITInterruptHandler(HW::InterruptData* /*unused*/) {
        if (sleepTimer == 0) {
            return;
        }

        sleepTimer--;
    }

}  // namespace FunnyOS::Bootloader