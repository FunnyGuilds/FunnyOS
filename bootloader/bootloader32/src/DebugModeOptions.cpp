#include "DebugModeOptions.hpp"

#include <FunnyOS/Hardware/Interrupts.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include "DebugMenu.hpp"

namespace FunnyOS::Bootloader32::DebugMenu {
    using namespace Stdlib;

    char* DebugModeInfo() {
        String::StringBuffer buffer = Memory::AllocateBuffer<char>(5);
        String::Concat(buffer, "", Bootloader::Logging::IsDebugModeEnabled() ? "ON" : "OFF");
        return buffer.Data;
    }

    void UpdateDebugMode() {
        Bootloader::Logging::SetDebugModeEnabled(!Bootloader::Logging::IsDebugModeEnabled());
    }

    void PrintMemoryMap() {
        HW::NoInterruptsBlock noInterrupts;

        Bootloader::Logging::GetTerminalManager()->ClearScreen();
        FB_LOG_INFO("Memory map");

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

            FB_LOG_INFO_F(" at 0x%016llx - 0x%016llx b - %s", entry->BaseAddress, entry->Length, type);
        }

        HW::PS2::ScanCode code;
        while (!HW::PS2::TryReadScanCode(code) || code != HW::PS2::ScanCode::Enter_Pressed) {
        }
    }

    void PrintBootloaderParameters() {
        HW::NoInterruptsBlock noInterrupts;

        Bootloader::Logging::GetTerminalManager()->ClearScreen();
        FB_LOG_INFO("Bootloader params");

        const auto& args = Bootloader::GetBootloader()->GetBootloaderParameters();

        FB_LOG_INFO_F("     - BootDriveNumber = 0x%02x", args.BootDriveNumber);
        FB_LOG_INFO_F("     - BootPartition = 0x%02x", args.BootPartition);
        FB_LOG_INFO_F("     - MemoryMapHasAcpiExtendedAttribute = %s",
                      args.MemoryMapHasAcpiExtendedAttribute ? "yes" : "no");
        FB_LOG_INFO_F("     - MemoryMapStart = 0x%04X", args.MemoryMapStart);
        FB_LOG_INFO_F("     - MemoryMapEntriesCount = %d", args.MemoryMapEntriesCount);


        HW::PS2::ScanCode code;
        while (!HW::PS2::TryReadScanCode(code) || code != HW::PS2::ScanCode::Enter_Pressed) {
        }
    }

    void QuitMenu() {
        HandleKey(HW::PS2::ScanCode::Escape_Released);
    }
}  // namespace FunnyOS::Bootloader32::DebugMenu