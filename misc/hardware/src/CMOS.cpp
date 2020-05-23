#include <FunnyOS/Hardware/CMOS.hpp>

#include <FunnyOS/Hardware/InputOutput.hpp>
#include <FunnyOS/Hardware/Interrupts.hpp>

namespace FunnyOS::HW::CMOS {
    using namespace InputOutput;

    namespace {
        constexpr uint16_t CMOS_PORT_SELECT = 0x70;
        constexpr uint16_t CMOS_PORT_DATA   = CMOS_PORT_SELECT + 1;

        void SelectCMOSRegister(CMOSRegister reg) {
            OutputByte(CMOS_PORT_SELECT, (static_cast<uint8_t>(reg) & 0b0111'1111));
            IOWait();
        }

        constexpr uint8_t BcdToNumber(uint8_t bcd) {
            return (bcd / 16) * 10 + (bcd % 16);
        }
    }  // namespace

    uint8_t ReadCMOSRegister(CMOSRegister reg) {
        SelectCMOSRegister(reg);
        return InputByte(CMOS_PORT_DATA);
    }

    void WriteCMOSRegister(CMOSRegister reg, uint8_t value) {
        SelectCMOSRegister(reg);
        OutputByte(CMOS_PORT_DATA, value);
    }

    RTCTime FetchRTCTime() {
        HW::NoInterruptsBlock noInterrupts;

        // Select 24-h mode and BCD mode
        uint8_t statusB = ReadCMOSRegister(CMOSRegister::StatusB);
        statusB |= 0b0000'0010;
        statusB &= ~0b0000'0100;
        WriteCMOSRegister(CMOSRegister::StatusB, statusB);

        // Read date in a loop until we get 2 consistent reads
        RTCTime oldTime{};
        RTCTime newTime{};
        do {
            oldTime = newTime;

            // Wait if there's an update
            while ((ReadCMOSRegister(CMOSRegister::StatusA) & 0b1000'000) != 0) {
            }

            newTime.Year       = ReadCMOSRegister(CMOSRegister::Year);
            newTime.Month      = ReadCMOSRegister(CMOSRegister::Month);
            newTime.DayOfMonth = ReadCMOSRegister(CMOSRegister::DayOfMonth);
            newTime.Hours      = ReadCMOSRegister(CMOSRegister::Hours);
            newTime.Minutes    = ReadCMOSRegister(CMOSRegister::Minutes);

        } while ((newTime.Year != oldTime.Year) || (newTime.Month != oldTime.Month) ||
                 (newTime.DayOfMonth != oldTime.DayOfMonth) || (newTime.Hours != oldTime.Hours) ||
                 (newTime.Minutes != oldTime.Minutes));

        newTime.Year       = BcdToNumber(newTime.Year) + 2000;
        newTime.Month      = BcdToNumber(newTime.Month);
        newTime.DayOfMonth = BcdToNumber(newTime.DayOfMonth);
        newTime.Hours      = BcdToNumber(newTime.Hours);
        newTime.Minutes    = BcdToNumber(newTime.Minutes);

        return newTime;
    }

}  // namespace FunnyOS::HW::CMOS