#include <FunnyOS/Hardware/Serial.hpp>
#include <FunnyOS/Hardware/InputOutput.hpp>

namespace FunnyOS::HW::Serial {
    using namespace InputOutput;

    namespace {
        constexpr uint32_t MAX_BAUD = 115200;

        constexpr int DATA_REGISTER = 0;
        constexpr int INTERRUPT_ENABLED_REGISTER = 1;
        constexpr int BAUD_LOW_REGISTER = 0;
        constexpr int BAUD_HIGH_REGISTER = 1;
        constexpr int PENDING_REGISTER = 2;
        constexpr int LINE_CONTROL_REGISTER = 3;
        constexpr int MODEM_CONTROL_REGISTER = 4;
        constexpr int LINE_STATUS_REGISTER = 5;

        constexpr int DLAB_BIT = 0b1000'0000;

        inline uint16_t GetPortNumber(COMPort port) {
            switch (port) {
                case COMPort::COM1:
                    return 0x3F8;
                case COMPort::COM2:
                    return 0x2F8;
                case COMPort::COM3:
                    return 0x3E8;
                case COMPort::COM4:
                    return 0x2E8;
                default:
                    F_ASSERT(false, "Invalid COM port");
            }
        }

        void SetDLAB(size_t portNum, bool dlab) {
            uint8_t value = InputByte(portNum + LINE_CONTROL_REGISTER);
            value = dlab ? (value | DLAB_BIT) : (value & ~DLAB_BIT);
            OutputByte(portNum + LINE_CONTROL_REGISTER, value);
        }
    }  // namespace

    bool InitializeCOMPort(COMPort port, DataBits data, StopBits stop, ParityBits parity, uint32_t baud) {
        const size_t portNum = GetPortNumber(port);

        // Disable interrupts
        SetDLAB(portNum, false);
        OutputByte(portNum + INTERRUPT_ENABLED_REGISTER, 0);

        // Set baud rate
        if (MAX_BAUD % baud != 0) {
            return false;
        }

        const uint16_t baudDivisor = MAX_BAUD / baud;
        SetDLAB(portNum, true);
        OutputByte(portNum + BAUD_LOW_REGISTER, baudDivisor & 0xFF);
        OutputByte(portNum + BAUD_HIGH_REGISTER, (baudDivisor >> 8) & 0xFF);

        // Set bits, parity and stop
        const uint8_t mask = ((static_cast<uint8_t>(data) & 0b11) << 0) | ((static_cast<uint8_t>(stop) & 0b1) << 2) |
                             ((static_cast<uint8_t>(parity) & 0b111) << 3);
        OutputByte(portNum + LINE_CONTROL_REGISTER, mask);

        // Enable FIFO
        OutputByte(portNum + PENDING_REGISTER, 0b1100'0111);

        // Enable IRQs, RTS, DTS
        // http://synfare.com/599N105E/hwdocs/serial/serial04.html
        OutputByte(portNum + MODEM_CONTROL_REGISTER, 0b0001011);

        return true;
    }

    void SerialInterruptHandler(InterruptData* /*unused*/) {}

    bool CanWrite(COMPort port) {
        return (InputByte(GetPortNumber(port) + LINE_STATUS_REGISTER) & 0b0010'0000) != 0;
    }

    void Write(COMPort port, uint8_t byte) {
        OutputByte(GetPortNumber(port) + DATA_REGISTER, byte);
    }

    bool CanRead(COMPort port) {
        return (InputByte(GetPortNumber(port) + LINE_STATUS_REGISTER) & 0b0000'0001) != 0;
    }

    uint8_t Read(COMPort port) {
        return InputByte(GetPortNumber(port) + DATA_REGISTER);
    }
}  // namespace FunnyOS::HW::Serial
