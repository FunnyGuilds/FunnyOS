#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CMOS_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CMOS_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::HW::CMOS {

    /**
     * CMOS Register numbers
     */
    enum class CMOSRegister : uint8_t {
        /**
         * Current second in minute.
         */
        Seconds = 0x00,

        /**
         * Current minute in hour.
         */
        Minutes = 0x02,

        /**
         * Current hour in day.
         */
        Hours = 0x04,

        /**
         * Current day of the week.
         * 1 to 7, where 1 is Sunday
         */
        Weekday = 0x06,

        /**
         * Current day of the month.
         */
        DayOfMonth = 0x07,

        /**
         * Current month.
         */
        Month = 0x08,

        /*
         * Current year in a century.
         */
        Year = 0x09,

        /**
         * Status Register A
         */
        StatusA = 0x0A,

        /**
         * Status Register B
         */
        StatusB = 0x0B,
    };

    /**
     * Reads a value from a CMOS register
     *
     * @param reg register to read from
     *
     * @return read value
     */
    uint8_t ReadCMOSRegister(CMOSRegister reg);

    /**
     * Reads a value to a CMOS register
     *
     * @param reg register to write to
     * @param value value to write
     */
    void WriteCMOSRegister(CMOSRegister reg, uint8_t value);

    /**
     * Represents time read from the RTC clock.
     */
    struct RTCTime {
        /**
         * Current year.
         */
        uint16_t Year;

        /**
         * Current month.
         */
        uint8_t Month;

        /**
         * Current day of the month.
         */
        uint8_t DayOfMonth;

        /**
         * Current hour. (24-hours format, 0 - 23)
         */
        uint8_t Hours;

        /**
         * Current minute.
         */
        uint8_t Minutes;
    };

    /**
     * Tries to fetch a reliable result from the CMOS time registers.
     *
     * @return reliable read of the CMOS Real Time Clock.
     */
    RTCTime FetchRTCTime();

}  // namespace FunnyOS::HW::CMOS

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CMOS_HPP
