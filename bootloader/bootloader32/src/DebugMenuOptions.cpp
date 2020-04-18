#include "DebugMenuOptions.hpp"

#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include "DebugMenu.hpp"

namespace FunnyOS::Bootloader32::DebugMenu {
    using namespace Stdlib;

    void DebugModeOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Debug mode");
    }

    void DebugModeOption::FetchState(String::StringBuffer& buffer) const {
        String::Append(buffer, Bootloader::Logging::IsDebugModeEnabled() ? "ON" : "OFF");
    }

    void DebugModeOption::Enter() {
        Bootloader::Logging::SetDebugModeEnabled(!Bootloader::Logging::IsDebugModeEnabled());
        SelectCurrentSubmenu(-1);
    }

    void DebugModeOption::HandleKey(HW::PS2::ScanCode) {}

    DebugModeOption DebugModeOption::s_instance{};

    void PrintMemoryMapOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Print memory map");
    }

    void PrintMemoryMapOption::FetchState(String::StringBuffer&) const {}

    void PrintMemoryMapOption::Enter() {
        Bootloader::Logging::GetTerminalManager()->ClearScreen();

        const auto& args = Bootloader::GetBootloader()->GetBootloaderParameters();
        for (size_t i = 0; i < args.MemoryMapEntriesCount; i++) {
            auto* entry = reinterpret_cast<Bootloader::BootloaderParameters::MemoryMapEntry*>(args.MemoryMapStart) + i;
            const char* type;
            switch (entry->Type) {
                case Bootloader::BootloaderParameters::MemoryMapEntryType::AvailableMemory:
                    type = "Available";
                    break;
                case Bootloader::BootloaderParameters::MemoryMapEntryType::ReservedMemory:
                    type = "Reserved";
                    break;
                case Bootloader::BootloaderParameters::MemoryMapEntryType::ACPIReclaimMemory:
                    type = "ACPIReclaimable";
                    break;
                case Bootloader::BootloaderParameters::MemoryMapEntryType::ACPINVSMemory:
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
        Bootloader::Logging::GetTerminalManager()->ClearScreen();
        FB_LOG_INFO("Bootloader params");

        const auto& args = Bootloader::GetBootloader()->GetBootloaderParameters();

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