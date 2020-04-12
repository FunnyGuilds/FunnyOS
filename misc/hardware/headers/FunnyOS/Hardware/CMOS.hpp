#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CMOS_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CMOS_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::HW::CMOS {

    enum class CMOSRegister : uint8_t {
        Seconds = 0x00,
        Minutes = 0x02,
        Hours = 0x04,
        Weekday = 0x06,
        DayOfMonth = 0x07,
        Month = 0x08,
        Year = 0x09,
        StatusA = 0x0A,
        StatusB = 0x0B,
    };

    uint8_t ReadCMOSRegister(CMOSRegister reg);

    void WriteCMOSRegister(CMOSRegister reg, uint8_t value);

    struct RTCTime {
       uint16_t Year;
       uint8_t Month;
       uint8_t DayOfMonth;
       uint8_t Hours;
       uint8_t Minutes;
    };

    RTCTime FetchRTCTime();

}  // namespace FunnyOS::HW::CMOS

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CMOS_HPP
