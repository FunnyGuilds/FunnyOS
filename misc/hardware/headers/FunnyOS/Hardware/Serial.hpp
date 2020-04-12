#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_SERIAL_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_SERIAL_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include "Interrupts.hpp"

namespace FunnyOS::HW::Serial {
    enum class COMPort { COM1 = 1, COM2 = 2, COM3 = 3, COM4 = 4 };

    enum class DataBits {
        BITS_5 = 0,
        BITS_6 = 1,
        BITS_7 = 2,
        BITS_8 = 3,
    };

    enum class StopBits { STOP_1 = 0, STOP_2 = 1 };

    enum class ParityBits { NONE = 0, ODD = 1, EVEN = 3, MARK = 5, SPACE = 7 };

    bool InitializeCOMPort(COMPort port, DataBits data, StopBits stop, ParityBits parity, uint32_t baud);

    void SerialInterruptHandler(InterruptData *data);

    bool CanWrite(COMPort port);

    void Write(COMPort port, uint8_t byte);

    bool CanRead(COMPort port);

    uint8_t Read(COMPort port);

    // TODO: More robust reading and writing using IRQs

}  // namespace FunnyOS::HW::Serial

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_SERIAL_HPP
