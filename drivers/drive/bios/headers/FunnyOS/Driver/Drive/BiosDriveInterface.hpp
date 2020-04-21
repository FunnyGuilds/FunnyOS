#ifndef FUNNYOS_DRIVERS_DRIVE_BIOS_BIOSDRIVEINTERFACE_HPP
#define FUNNYOS_DRIVERS_DRIVE_BIOS_BIOSDRIVEINTERFACE_HPP

#include <FunnyOS/Hardware/Drive/DriveInterface.hpp>
#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Stdlib/System.hpp>

namespace FunnyOS::Driver::Drive {
    using namespace HW;
    using namespace Stdlib;

    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(BiosException);

    class BiosDriveInterface : public IDriveInterface {
       public:
        explicit BiosDriveInterface(DriveIdentification drive);

       public:
        DriveIdentification GetDriveIdentification() const override;

        SectorNumber GetTotalSectorCount() const override;

        size_t GetSectorSize() const override;

        void ReadSectors(SectorNumber sector, SectorNumber count, Memory::SizedBuffer<uint8_t>& buffer) override;

        bool HasExtendedDiskAccess() const;

        bool HasEnhancedDiskDriveFunctions() const;

        bool SupportsFlat64Addresses() const;

        uint8_t GetSectorsPerTrack() const;

        uint16_t GetMaxCylinderNumber() const;

        uint8_t GetHeadsPerCylinder() const;

       private:
        void Query();

        void DoExtendedRead(const char* when);

       private:
        DriveIdentification m_drive;
        SectorNumber m_sectorCount;
        size_t m_sectorSize;
        bool m_hasExtendedDiskAccess;
        bool m_hasEnhancedDiskDriveFunctions;
        bool m_supportsFlat64Addresses;
        uint8_t m_sectorsPerTrack;
        uint16_t m_maxCylinderNumber;
        uint8_t m_headsPerCylinder;
    };

}  // namespace FunnyOS::Driver::Drive

#endif  // FUNNYOS_DRIVERS_DRIVE_BIOS_BIOSDRIVEINTERFACE_HPP
