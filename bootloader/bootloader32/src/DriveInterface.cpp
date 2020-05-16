#include "DriveInterface.hpp"

#include <FunnyOS/Stdlib/Algorithm.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
#include "RealMode.hpp"

namespace FunnyOS::Bootloader32 {
    struct EDDParameters {
        uint16_t BufferSize;
        uint16_t InformationFlags;
        uint32_t NumberOfCylinders;
        uint32_t NumberOfHeads;
        uint32_t NumberOfSectors;
        uint64_t TotalNumberOfSectors;
        uint16_t BytesPerSector;
        uint8_t Ignored[40];
    } F_DONT_ALIGN;

    struct EDDDiskPacket {
        uint8_t PacketSize;
        uint8_t Reserved;
        uint8_t NumberOfBlocks;
        uint8_t Reserved2;
        uint16_t DestinationOffset;
        uint16_t DestinationSegment;
        uint64_t StartLBA;
        uint64_t Address64;
        uint32_t Unused[2];
    } F_DONT_ALIGN;

    F_SECTION(".real.data.edd") EDDDiskPacket g_eddPacket;

    namespace {
        constexpr SectorNumber DEFAULT_SECTOR_SIZE = 0x200;
        constexpr SectorNumber EDD_MAX_READ = 0x7F;

        char errorBuffer[128];

        void CheckErrors(const char* when, const Registers32& regs) {
            if ((regs.EFLAGS.Value16 & static_cast<uint16_t>(CPU::Flags::CarryFlag)) == 0) {
                return;
            }

            String::StringBuffer buffer{errorBuffer, 128};
            String::Format(buffer, "BIOS error when %s. CF is set. AH = 0x%02x", when, regs.EAX.Value8.High);
            F_ERROR_WITH_MESSAGE(DriveInterfaceException, errorBuffer);
        }
    }  // namespace

    DriveInterface::DriveInterface(DriveIdentification drive) : m_drive(drive) {
        Query();
    }

    DriveIdentification DriveInterface::GetDriveIdentification() const {
        return m_drive;
    }

    SectorNumber DriveInterface::GetTotalSectorCount() const {
        return m_sectorCount;
    }

    size_t DriveInterface::GetSectorSize() const {
        return m_sectorSize;
    }

    void DriveInterface::ReadSectors(SectorNumber sector, SectorNumber count,
                                         Memory::SizedBuffer<uint8_t>& buffer) {
        if (buffer.Size < count * GetSectorSize()) {
            F_ERROR_WITH_MESSAGE(DriveInterfaceException, "buffer to small to hold all sectors");
        }

        SectorNumber currentOffset = 0;
        SectorNumber leftToRead = count;

        if (m_supportsFlat64Addresses) {
            while (leftToRead > 0) {
                const SectorNumber currentReadSize = Min(leftToRead, EDD_MAX_READ);
                g_eddPacket.NumberOfBlocks = currentReadSize;
                g_eddPacket.DestinationSegment = 0xFFFF;
                g_eddPacket.DestinationOffset = 0xFFFF;
                g_eddPacket.StartLBA = sector + currentOffset;
                g_eddPacket.Address64 = reinterpret_cast<uint64_t>(buffer.Data + currentOffset * m_sectorSize);
                DoExtendedRead("reading using EDD with flat 64-bit address");

                leftToRead -= currentReadSize;
                currentOffset += currentReadSize;
            }

            return;
        }
        const SectorNumber bufferSize = GetRealModeBuffer().Size / m_sectorSize;

        if (m_hasExtendedDiskAccess) {
            auto& buf = GetRealModeBuffer();
            F_ASSERT(buf.Size >= m_sectorSize, "real mode buffer to small to hold a sector");

            while (leftToRead > 0) {
                const SectorNumber currentReadSize = Min(leftToRead, EDD_MAX_READ, bufferSize);
                uint16_t destinationSegment = 0;
                uint16_t destinationOffset = 0;
                GetRealModeBufferAddress(destinationSegment, destinationOffset);
                g_eddPacket.DestinationSegment = destinationSegment;
                g_eddPacket.DestinationOffset = destinationOffset;

                g_eddPacket.NumberOfBlocks = currentReadSize;
                g_eddPacket.StartLBA = sector + currentOffset;
                g_eddPacket.Address64 = 0;
                DoExtendedRead("reading using EDD with non-flat address");

                Memory::Copy(buffer.Data + currentOffset * m_sectorSize, buf.Data, currentReadSize * m_sectorSize);
                leftToRead -= currentReadSize;
                currentOffset += currentReadSize;
            }

            return;
        }

        while (leftToRead > 0) {
            const SectorNumber currentLba = sector + currentOffset;
            const uint16_t cylinder = currentLba / (m_headsPerCylinder * m_sectorsPerTrack);
            const uint8_t head = (currentLba / m_sectorsPerTrack) % m_headsPerCylinder;
            const uint8_t currentSector = (currentLba % m_sectorsPerTrack) + 1;
            F_ASSERT(currentSector <= 63, "sector > 63");

            const SectorNumber leftSectorsOnTrack = m_sectorsPerTrack - sector + 1;
            const SectorNumber currentReadSize = Min(leftToRead, leftSectorsOnTrack, bufferSize);

            Registers32 regs;
            regs.EAX.Value8.High = 0x02;
            regs.EAX.Value8.Low = currentReadSize;
            regs.ECX.Value8.High = cylinder & 0xFF;
            regs.ECX.Value8.Low = (((cylinder >> 8) & 0b11) << 6) | (currentSector & 0b00111111);
            regs.EDX.Value8.High = head;
            regs.EDX.Value8.Low = m_drive;

            uint16_t bufferSegment = 0;
            uint16_t bufferSegmentOffset = 0;
            GetRealModeBufferAddress(bufferSegment, bufferSegmentOffset);
            regs.ES.Value16 = bufferSegment;
            regs.EBX.Value16 = bufferSegmentOffset;

            RealModeInt(0x13, regs);
            CheckErrors("reading via INT13 AH=0x02", regs);
            Memory::Copy(buffer.Data + currentOffset * m_sectorSize, GetRealModeBuffer().Data,
                         currentReadSize * m_sectorSize);

            leftToRead -= currentReadSize;
            currentOffset += currentReadSize;
        }
    }

    void DriveInterface::Query() {
        // Int13 extension installation check
        Registers32 regs;
        regs.EAX.Value8.High = 0x41;
        regs.EBX.Value16 = 0x55AA;
        regs.EDX.Value8.Low = m_drive;
        RealModeInt(0x13, regs);

        if ((regs.EFLAGS.Value16 & static_cast<uint16_t>(CPU::Flags::CarryFlag)) == 0 && regs.EBX.Value16 == 0xAA55) {
            m_hasExtendedDiskAccess = (regs.ECX.Value16 & 0b1) != 0;
            m_hasEnhancedDiskDriveFunctions = (regs.ECX.Value16 & 0b100) != 0;
            m_supportsFlat64Addresses = (regs.ECX.Value16 & 0b1000) != 0;
        } else {
            m_hasExtendedDiskAccess = false;
            m_hasEnhancedDiskDriveFunctions = false;
            m_supportsFlat64Addresses = false;
        }

        if (m_hasExtendedDiskAccess || m_hasEnhancedDiskDriveFunctions) {
            F_ASSERT(GetRealModeBuffer().Size >= sizeof(EDDParameters),
                     "real mode buffer to small to hold EDD parameters");

            auto* parameters = reinterpret_cast<EDDParameters*>(GetRealModeBuffer().Data);
            parameters->BufferSize = sizeof(EDDParameters);
            regs.EAX.Value8.High = 0x48;
            regs.EDX.Value8.Low = m_drive;
            regs.ESI.Value16 = static_cast<uint16_t>(reinterpret_cast<uintptr_t>(parameters));
            RealModeInt(0x13, regs);
            CheckErrors("reading EDD", regs);

            m_sectorSize = parameters->BytesPerSector;
            m_sectorCount = parameters->TotalNumberOfSectors;
            m_sectorsPerTrack = parameters->NumberOfSectors;
            m_maxCylinderNumber = parameters->NumberOfCylinders;
            m_headsPerCylinder = parameters->NumberOfHeads;
        } else {
            m_sectorSize = DEFAULT_SECTOR_SIZE;

            regs.EAX.Value8.High = 0x08;
            regs.EDX.Value8.Low = m_drive;
            regs.EDI.Value16 = 0;
            RealModeInt(0x13, regs);
            CheckErrors("getting drive parameters", regs);

            m_sectorsPerTrack = regs.ECX.Value8.Low & 0b0011'1111;
            m_maxCylinderNumber = ((((regs.ECX.Value8.Low & 0b1100'0000) >> 6) << 8) | regs.ECX.Value8.High) + 1;
            m_headsPerCylinder = regs.EDX.Value8.High + 1;
            m_sectorCount = m_sectorsPerTrack * m_headsPerCylinder * m_maxCylinderNumber;
        }
    }

    void DriveInterface::DoExtendedRead(const char* when) const {
        g_eddPacket.PacketSize = sizeof(EDDDiskPacket);
        g_eddPacket.Reserved = 0;
        g_eddPacket.Unused[0] = 0;
        g_eddPacket.Unused[1] = 0;

        Registers32 regs;
        regs.EAX.Value8.High = 0x42;
        regs.EBX.Value16 = 0x55AA;
        regs.ESI.Value16 = static_cast<uint16_t>(reinterpret_cast<uintptr_t>(&g_eddPacket));
        regs.EDX.Value8.Low = m_drive;
        RealModeInt(0x13, regs);
        CheckErrors(when, regs);
    }

    bool DriveInterface::HasExtendedDiskAccess() const {
        return m_hasExtendedDiskAccess;
    }

    bool DriveInterface::HasEnhancedDiskDriveFunctions() const {
        return m_hasEnhancedDiskDriveFunctions;
    }

    bool DriveInterface::SupportsFlat64Addresses() const {
        return m_supportsFlat64Addresses;
    }

    uint8_t DriveInterface::GetSectorsPerTrack() const {
        return m_sectorsPerTrack;
    }

    uint16_t DriveInterface::GetMaxCylinderNumber() const {
        return m_maxCylinderNumber;
    }

    uint8_t DriveInterface::GetHeadsPerCylinder() const {
        return m_headsPerCylinder;
    }
}  // namespace FunnyOS::Bootloader32