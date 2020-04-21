#include "DebugMenuOptions.hpp"

#include <FunnyOS/Driver/Drive/BiosDriveInterface.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include "Bootloader32.hpp"
#include "DebugMenu.hpp"

namespace FunnyOS::Bootloader32::DebugMenu {
    using namespace Stdlib;

    void DebugModeOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Debug mode");
    }

    void DebugModeOption::FetchState(String::StringBuffer& buffer) const {
        String::Append(buffer, Logging::IsDebugModeEnabled() ? "ON" : "OFF");
    }

    void DebugModeOption::Enter() {
        Logging::SetDebugModeEnabled(!Logging::IsDebugModeEnabled());
        SelectCurrentSubmenu(-1);
    }

    void DebugModeOption::HandleKey(HW::PS2::ScanCode) {}

    DebugModeOption DebugModeOption::s_instance{};

    void PrintMemoryMapOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Print memory map");
    }

    void PrintMemoryMapOption::FetchState(String::StringBuffer&) const {}

    void PrintMemoryMapOption::Enter() {
        Logging::GetTerminalManager()->ClearScreen();

        const auto& args = Bootloader::Get().GetBootloaderParameters();
        for (size_t i = 0; i < args.MemoryMapEntriesCount; i++) {
            auto* entry = reinterpret_cast<BootloaderParameters::MemoryMapEntry*>(args.MemoryMapStart) + i;
            const char* type;
            switch (entry->Type) {
                case BootloaderParameters::MemoryMapEntryType::AvailableMemory:
                    type = "Available";
                    break;
                case BootloaderParameters::MemoryMapEntryType::ReservedMemory:
                    type = "Reserved";
                    break;
                case BootloaderParameters::MemoryMapEntryType::ACPIReclaimMemory:
                    type = "ACPIReclaimable";
                    break;
                case BootloaderParameters::MemoryMapEntryType::ACPINVSMemory:
                    type = "ACPINVS";
                    break;
                default:
                    type = "Unknown";
                    break;
            }

            char acpi[8] = "N/A";
            if (args.MemoryMapHasAcpiExtendedAttribute) {
                String::StringBuffer acpiBuffer{acpi, 8};
                String::IntegerToString(acpiBuffer, entry->ACPIFlags, 2);
            }

            FB_LOG_INFO_F("O 0x%016llx L 0x%016llx T %15s A %02s", entry->BaseAddress, entry->Length, type, acpi);
        }
    }

    void PrintMemoryMapOption::HandleKey(HW::PS2::ScanCode code) {
        if (code == HW::PS2::ScanCode::Enter_Released) {
            SelectCurrentSubmenu(-1);
        }
    }

    PrintMemoryMapOption PrintMemoryMapOption::s_instance{};

    void PrintBootloaderParametersOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Print bootloader parameters");
    }

    void PrintBootloaderParametersOption::FetchState(String::StringBuffer&) const {}

    void PrintBootloaderParametersOption::Enter() {
        Logging::GetTerminalManager()->ClearScreen();
        FB_LOG_INFO("Bootloader params");

        const auto& args = Bootloader::Get().GetBootloaderParameters();

        FB_LOG_INFO_F("     - BootDriveNumber = 0x%02x", args.BootDriveNumber);
        FB_LOG_INFO_F("     - BootPartition = 0x%02x", args.BootPartition);
        FB_LOG_INFO_F("     - MemoryMapHasAcpiExtendedAttribute = %s",
                      args.MemoryMapHasAcpiExtendedAttribute ? "yes" : "no");
        FB_LOG_INFO_F("     - MemoryMapStart = 0x%04X", args.MemoryMapStart);
        FB_LOG_INFO_F("     - MemoryMapEntriesCount = %d", args.MemoryMapEntriesCount);
    }

    void PrintBootloaderParametersOption::HandleKey(HW::PS2::ScanCode code) {
        if (code == HW::PS2::ScanCode::Enter_Released) {
            SelectCurrentSubmenu(-1);
        }
    }

    PrintBootloaderParametersOption PrintBootloaderParametersOption::s_instance{};

    void PrintBootDiskParameters::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Print bootable drive information");
    }

    void PrintBootDiskParameters::FetchState(String::StringBuffer&) const {}

    void PrintBootDiskParameters::Enter() {
        Logging::GetTerminalManager()->ClearScreen();

        const auto& args = Bootloader::Get().GetBootloaderParameters();
        Driver::Drive::BiosDriveInterface interface(args.BootDriveNumber);

        FB_LOG_INFO("Boot drive parameters");
        FB_LOG_INFO_F("     - DriveIdentification = 0x%02x", interface.GetDriveIdentification());
        FB_LOG_INFO_F("     - TotalSectorCount = 0x%02x", interface.GetTotalSectorCount());
        FB_LOG_INFO_F("     - SectorSize = 0x%04x", interface.GetSectorSize());
        FB_LOG_INFO_F("     - HasExtendedDiskAccess = %s", interface.HasExtendedDiskAccess() ? "yes" : "no");
        FB_LOG_INFO_F("     - HasEnhancedDiskDriveFunctions = %s",
                      interface.HasEnhancedDiskDriveFunctions() ? "yes" : "no");
        FB_LOG_INFO_F("     - SupportsFlat64Addresses = %s", interface.SupportsFlat64Addresses() ? "yes" : "no");
        FB_LOG_INFO_F("     - SectorsPerTrack = 0x%02x", interface.GetSectorsPerTrack());
        FB_LOG_INFO_F("     - HeadsPerCylinder = 0x%02x", interface.GetHeadsPerCylinder());
        FB_LOG_INFO_F("     - MaxCylinderNumber = 0x%04x", interface.GetMaxCylinderNumber());
    }

    void PrintBootDiskParameters::HandleKey(HW::PS2::ScanCode code) {
        if (code == HW::PS2::ScanCode::Enter_Released) {
            SelectCurrentSubmenu(-1);
        }
    }

    PrintBootDiskParameters PrintBootDiskParameters::s_instance{};

    void QuitMenuOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Quit menu");
    }

    void QuitMenuOption::FetchState(String::StringBuffer&) const {}

    void QuitMenuOption::Enter() {
        SelectCurrentSubmenu(-1);
        DebugMenu::HandleKey(HW::PS2::ScanCode::Escape_Released);
    }

    void QuitMenuOption::HandleKey(HW::PS2::ScanCode) {}

    QuitMenuOption QuitMenuOption::s_instance{};

}  // namespace FunnyOS::Bootloader32::DebugMenu