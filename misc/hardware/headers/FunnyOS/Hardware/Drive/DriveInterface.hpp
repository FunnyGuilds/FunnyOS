#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_DRIVEINTERFACE_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_DRIVEINTERFACE_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>

namespace FunnyOS::HW {
    using namespace Stdlib;

    using DriveIdentification = uint8_t;
    using SectorNumber = uint32_t;

    class IDriveInterface {
       public:
        virtual DriveIdentification GetDriveIdentification() const = 0;

        virtual SectorNumber GetTotalSectorCount() const = 0;

        virtual size_t GetSectorSize() const = 0;

        virtual void ReadSectors(SectorNumber sector, SectorNumber count, Memory::SizedBuffer<uint8_t>& buffer) = 0;
    };

}  // namespace FunnyOS::HW

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_DRIVEINTERFACE_HPP
