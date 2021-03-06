#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_DRIVEINTERFACE_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_DRIVEINTERFACE_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/System.hpp>

namespace FunnyOS::HW {
    using namespace Stdlib;

    using drive_indentification_t = uint8_t;
    using sectornumber_t          = uint32_t;

    /**
     * General exception thrown by IDriveInterface.
     */
    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(DriveInterfaceException);

    class IDriveInterface {
        INTERFACE(IDriveInterface);

       public:
        /**
         * Gets a unique ID identifying this drive.
         * This ID is not bound to a hardware rather than to a specific slot that the drive is connected to.
         *
         * @return unique ID identifying this drive.
         */
        [[nodiscard]] virtual drive_indentification_t GetDriveIdentification() const = 0;

        /**
         * Gets the total amount of sectors on the drive.
         *
         * @return total amount of sectors on the drive.
         */
        [[nodiscard]] virtual sectornumber_t GetTotalSectorCount() const = 0;

        /**
         * Gets the size of a single sector in bytes.
         *
         * @return  size of a single sector in bytes.
         */
        [[nodiscard]] virtual size_t GetSectorSize() const = 0;

        /**
         * Reads sector from drive into memory.
         *
         * @param sector logical block address of the first sector to read
         * @param count amount of sectors to read
         * @param buffer destination memory
         *
         * @throws DriveInterfaceException if the read failed or the specified buffer was too small.
         */
        virtual void ReadSectors(sectornumber_t sector, sectornumber_t count, Memory::SizedBuffer<uint8_t>& buffer) = 0;
    };

}  // namespace FunnyOS::HW

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_DRIVEINTERFACE_HPP
