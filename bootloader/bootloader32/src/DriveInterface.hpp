#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DRIVEINTERFACE_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DRIVEINTERFACE_HPP

#include <FunnyOS/Hardware/Drive/DriveInterface.hpp>
#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Stdlib/System.hpp>

namespace FunnyOS::Bootloader32 {
    using namespace HW;
    using namespace Stdlib;

    /**
     * DriveInterface implementation for disk IO using BIOS int13
     *
     * Supports and defaults to the Enhanced Disk Drive 3.0 specification if available.
     */
    class DriveInterface : public IDriveInterface {
       public:
        /**
         * Setups a new drive interface for the drive identified by the given ID.
         *
         * @param drive drive identification number.
         */
        explicit DriveInterface(drive_indentification_t drive);

       public:
        [[nodiscard]] drive_indentification_t GetDriveIdentification() const override;
        [[nodiscard]] sectornumber_t GetTotalSectorCount() const override;
        [[nodiscard]] size_t GetSectorSize() const override;
        void ReadSectors(sectornumber_t sector, sectornumber_t count, Memory::SizedBuffer<uint8_t>& buffer) override;

        /**
         * Returns whether or not the EDD Fixed Disk Access Subset is available.
         *
         * @return whether or not the EDD Fixed Disk Access Subset is available.
         */
        [[nodiscard]] bool HasExtendedDiskAccess() const;

        /**
         * Returns whether or not the Enhanced Disk Drive (EDD) Support Subset is available.
         *
         * @return whether or not the Enhanced Disk Drive (EDD) Support Subset is available.
         */
        [[nodiscard]] bool HasEnhancedDiskDriveFunctions() const;

        /**
         * Returns whether or not flat 64-bit addresses are supported in EDD disk address packet.
         *
         * @return whether or not flat 64-bit addresses are supported in EDD disk address packet.
         */
        [[nodiscard]] bool SupportsFlat64Addresses() const;

        /**
         * Returns the amount of sectors per single track on the drive.
         *
         * @return  the amount of sectors per single track on the drive.
         */
        [[nodiscard]] uint8_t GetSectorsPerTrack() const;

        /**
         * Returns the maximum cylinder number on the drive.
         *
         * @return the maximum cylinder number on the drive.
         */
        [[nodiscard]] uint16_t GetMaxCylinderNumber() const;

        /**
         * Returns the amount of heads per single cylinder on the drive.
         *
         * @return  the amount of heads per single cylinder on the drive.
         */
        [[nodiscard]] uint8_t GetHeadsPerCylinder() const;

       private:
        void Query();

        void DoExtendedRead(const char* when) const;

       private:
        drive_indentification_t m_drive;
        sectornumber_t m_sectorCount         = 0;
        size_t m_sectorSize                  = 0;
        bool m_hasExtendedDiskAccess         = false;
        bool m_hasEnhancedDiskDriveFunctions = false;
        bool m_supportsFlat64Addresses       = false;
        uint8_t m_sectorsPerTrack            = 0;
        uint16_t m_maxCylinderNumber         = 0;
        uint8_t m_headsPerCylinder           = 0;
    };

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DRIVEINTERFACE_HPP
