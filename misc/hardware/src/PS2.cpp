#include <FunnyOS/Hardware/PS2.hpp>

#include <FunnyOS/Hardware/InputOutput.hpp>
#include <FunnyOS/Hardware/Interrupts.hpp>

namespace FunnyOS::HW::PS2 {
    namespace {
        constexpr const uint16_t PS2_PORT_DATA = 0x60;
        constexpr const uint16_t PS2_PORT_STATUS = 0x64;
        constexpr const uint16_t PS2_PORT_COMMAND = 0x64;

        namespace StatusFlags {
            /**
             * Set when output buffer is full.
             * Must be set before attempting to read data from data port.
             */
            constexpr const uint8_t OutputBufferStatus = 1 << 0;

            /**
             * Set when input buffer is full.
             * Must be clear before attempting to read data from data port.
             */
            constexpr const uint8_t InputBufferStatus = 1 << 1;

            /**
             * Set by firmware when the system passes self test.
             */
            constexpr const uint8_t SystemFlag = 1 << 2;

            /**
             * If set data written to input buffer is data for PS/2 controller.
             * If clear data written to input buffer is data for PS/2 device.
             */
            constexpr const uint8_t DataForController = 1 << 3;

            /**
             * Chipset specific.
             */
            constexpr const uint8_t Unknown4 = 1 << 4;

            /**
             * Chipset specific.
             */
            constexpr const uint8_t Unknown5 = 1 << 5;

            /**
             * Set if a time-out error ocurred.
             */
            constexpr const uint8_t TimeOutError = 1 << 6;

            /**
             * Set if a parity error ocurred.
             */
            constexpr const uint8_t ParityError = 1 << 7;
        }  // namespace StatusFlags

        namespace ControllerCommands {
            /**
             * Disables first PS/2 port.
             */
            constexpr const uint8_t DisableFirstPort = 0xAD;

            /**
             * Enables first PS/2 port.
             */
            constexpr const uint8_t EnableFirstPort = 0xAE;

            /**
             * Read Controller Output Port.
             */
            constexpr const uint8_t ReadControllerOutputPort = 0xD0;

            /**
             * Write next byte to Controller Output Port.
             */
            constexpr const uint8_t WriteControllerOutputPort = 0xD1;
        }  // namespace ControllerCommands

        namespace KeyboardCommands {
            /**
             * Read the current scan code (if the following data is 0)
             * Or set the next scan code (if the following data is 1-3)
             */
            constexpr const uint8_t KeyboardSetScancode = 0xF0;
        }  // namespace KeyboardCommands

        namespace Response {
            constexpr uint8_t Acknowledged = 0xFA;
            constexpr uint8_t Resend = 0xFE;
        }  // namespace Response

        namespace ControllerOutputPort {
            /**
             * A20Gate (output)
             */
            constexpr const uint8_t A20Gate = 1 << 1;
        }  // namespace ControllerOutputPort

        uint8_t ReadData() {
            return InputOutput::InputByte(PS2_PORT_DATA);
        }

        void WriteData(uint8_t data) {
            InputOutput::OutputByte(PS2_PORT_DATA, data);
        }

        uint8_t ReadStatus() {
            return InputOutput::InputByte(PS2_PORT_STATUS);
        }

        void SendCommand(uint8_t data) {
            InputOutput::OutputByte(PS2_PORT_COMMAND, data);
        }

        bool CanRead() {
            return (ReadStatus() & StatusFlags::OutputBufferStatus) == 1;
        }

        bool CanWrite() {
            return (ReadStatus() & StatusFlags::InputBufferStatus) == 0;
        }

        void SendCommandBlocking(uint8_t data) {
            while (!CanWrite()) {
            }

            SendCommand(data);
        }

        void WriteDataBlocking(uint8_t data) {
            while (!CanWrite()) {
            }

            WriteData(data);
        }

        uint8_t ReadDataBlocking() {
            while (!CanRead()) {
            }

            return ReadData();
        }

        const uint8_t PRINTSCREEN_PRESSED_SCANCODE[] = {0xE0, 0x2A, 0xE0, 0x37};
        const uint8_t PRINTSCREEN_RELEASED_SCANCODE[] = {0xE0, 0xB7, 0xE0, 0xAA};
        const uint8_t PAUSE_PRESSED_SCANCODE[] = {0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5};

    }  // namespace

    void EnableA20() {
        HW::NoInterruptsBlock noInterrupts;
        SendCommandBlocking(ControllerCommands::DisableFirstPort);

        SendCommandBlocking(ControllerCommands::ReadControllerOutputPort);
        uint8_t controllerOutputPort = ReadDataBlocking();

        controllerOutputPort |= ControllerOutputPort::A20Gate;

        SendCommandBlocking(ControllerCommands::WriteControllerOutputPort);
        WriteDataBlocking(controllerOutputPort);

        SendCommandBlocking(ControllerCommands::EnableFirstPort);
    }

    bool InitializeKeyboard() {
        HW::NoInterruptsBlock noInterrupts;

        // Read any junk that can be in the register (for example by user mashing buttons)
        while (CanRead()) {
            ReadData();
        }

        // Set scan code 2
        WriteDataBlocking(KeyboardCommands::KeyboardSetScancode);
        WriteDataBlocking(2);

        const uint8_t data = ReadDataBlocking();
        return data == Response::Acknowledged;
    }

    namespace {
        static uint16_t g_scanCodeBuffer[6];
        static unsigned int g_scanCodeBufPosition = 0;
    }  // namespace

    bool TestForSpecialScanCode(const uint8_t* match, size_t size) {
        for (size_t i = 0; i < size; i++) {
            if (i > g_scanCodeBufPosition) {
                return false;
            }

            if (g_scanCodeBuffer[i] != match[i]) {
                return false;
            }
        }

        return true;
    }

    bool TryReadScanCode(ScanCode& code) {
        while (CanRead()) {
            const uint8_t value = ReadData();
            if (value == Response::Acknowledged) {
                return false;
            }

            if (g_scanCodeBufPosition >= 6) {
                g_scanCodeBufPosition = 0;
            }

            g_scanCodeBuffer[g_scanCodeBufPosition] = value;

            if (g_scanCodeBufPosition == 0 && value != 0xE0 && value != 0xE1) {
                // One-byte scan code
                g_scanCodeBufPosition = 0;
                code = static_cast<ScanCode>(g_scanCodeBuffer[0]);
                return true;
            }

            if (g_scanCodeBufPosition == 1 && g_scanCodeBuffer[0] == 0xE0) {
                const uint8_t part = g_scanCodeBuffer[1];

                // Parts of print screen scan code
                if (part == 0x2A || part == 0xB7) {
                    g_scanCodeBufPosition++;
                    return false;
                }

                // We have complete 0xE0XX scan code
                g_scanCodeBufPosition = 0;
                code = static_cast<ScanCode>(0xE000 | g_scanCodeBuffer[1]);
                return true;
            }

            // Other, special scan codes
            if (TestForSpecialScanCode(PRINTSCREEN_PRESSED_SCANCODE, sizeof(PRINTSCREEN_PRESSED_SCANCODE))) {
                g_scanCodeBufPosition = 0;
                code = ScanCode::PrintScreen_Pressed;
                return true;
            }

            if (TestForSpecialScanCode(PRINTSCREEN_RELEASED_SCANCODE, sizeof(PRINTSCREEN_RELEASED_SCANCODE))) {
                g_scanCodeBufPosition = 0;
                code = ScanCode::PrintScreen_Release;
                return true;
            }

            if (TestForSpecialScanCode(PAUSE_PRESSED_SCANCODE, sizeof(PAUSE_PRESSED_SCANCODE))) {
                g_scanCodeBufPosition = 0;
                code = ScanCode::Pause_Pressed;
                return true;
            }

            g_scanCodeBufPosition++;
        }

        return false;
    }
}  // namespace FunnyOS::HW::PS2