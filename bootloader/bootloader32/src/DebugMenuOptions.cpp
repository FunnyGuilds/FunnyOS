#include "DebugMenuOptions.hpp"

#include <FunnyOS/Driver/Drive/BiosDriveInterface.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
#include "DebugMenu.hpp"
#include "ElfLoader.hpp"
#include "FileLoader.hpp"

namespace FunnyOS::Bootloader32::DebugMenu {
    using namespace Stdlib;

    void SimpleSwitchModeOption::FetchState(String::StringBuffer& buffer) const {
        String::Append(buffer, GetMode() ? "ON" : "OFF");
    }

    void SimpleSwitchModeOption::Enter() {
        SetMode(!GetMode());
        SelectCurrentSubmenu(-1);
    }

    void SimpleSwitchModeOption::HandleKey(HW::PS2::ScanCode /*code*/) {}

    void DebugModeOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Debug mode");
    }

    void DebugModeOption::SetMode(bool mode) {
        Logging::SetDebugModeEnabled(mode);
    }

    bool DebugModeOption::GetMode() const {
        return Logging::IsDebugModeEnabled();
    }

    void LogToSerial::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Log messages to COM1");
    }

    void LogToSerial::SetMode(bool mode) {
        Logging::SetSerialLoggingEnabled(mode);
    }

    bool LogToSerial::GetMode() const {
        return Logging::IsSerialLoggingEnabled();
    }

    void DebugDiskIOOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Debug disk I/O");
    }

    void DebugDiskIOOption::SetMode(bool mode) {
        FileLoader::SetDebugReads(mode);

        if (mode) {
            Logging::SetDebugModeEnabled(true);
        }
    }

    bool DebugDiskIOOption::GetMode() const {
        return FileLoader::IsDebugReads();
    }

    void DebugElfLoaderOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Debug ElfLoader");
    }

    void DebugElfLoaderOption::SetMode(bool mode) {
        ElfLoader::SetDebugElfs(mode);

        if (mode) {
            Logging::SetDebugModeEnabled(true);
        }
    }

    bool DebugElfLoaderOption::GetMode() const {
        return ElfLoader::IsDebugElfs();
    }

    void PauseBeforeBootOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Pause before booting");
    }

    void PauseBeforeBootOption::SetMode(bool mode) {
        Bootloader::Get().SetPauseBeforeBoot(mode);
    }

    bool PauseBeforeBootOption::GetMode() const {
        return Bootloader::Get().IsPauseBeforeBoot();
    }

    void PrintMemoryMapOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Print memory map");
    }

    void PrintMemoryMapOption::FetchState(String::StringBuffer& /*buffer*/) const {}

    void PrintMemoryMapOption::Enter() {
        using Bootparams::MemoryMapEntryType;
        Logging::GetTerminalManager()->ClearScreen();
        const auto& memoryMap = Bootloader::Get().GetBootloaderParameters().MemoryMap;

        for (size_t i = 0; i < memoryMap.Count; i++) {
            const auto& entry = memoryMap[i];

            const char* type;
            switch (entry.Type) {
                case MemoryMapEntryType::AvailableMemory:
                    type = "Available";
                    break;
                case MemoryMapEntryType::ReservedMemory:
                    type = "Reserved";
                    break;
                case MemoryMapEntryType::ACPIReclaimMemory:
                    type = "ACPIReclaimable";
                    break;
                case MemoryMapEntryType::ACPINVSMemory:
                    type = "ACPINVS";
                    break;
                default:
                    type = "Unknown";
                    break;
            }

            char acpi[8] = "N/A";
            if (memoryMap.HasAcpiExtendedAttribute) {
                String::StringBuffer acpiBuffer{acpi, 8};
                String::IntegerToString(acpiBuffer, entry.ACPIFlags, 2);
            }

            FB_LOG_INFO_F("O 0x%016llx L 0x%016llx T %15s A %02s", entry.BaseAddress, entry.Length, type, acpi);
        }
    }

    void PrintMemoryMapOption::HandleKey(HW::PS2::ScanCode code) {
        if (code == HW::PS2::ScanCode::Enter_Released) {
            SelectCurrentSubmenu(-1);
        }
    }

    void PrintBootloaderParametersOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Print bootloader parameters");
    }

    void PrintBootloaderParametersOption::FetchState(String::StringBuffer& /*buffer*/) const {}

    void PrintBootloaderParametersOption::Enter() {
        Logging::GetTerminalManager()->ClearScreen();
        FB_LOG_INFO("Bootloader params");

        const auto& args = Bootloader::Get().GetBootloaderParameters();

        FB_LOG_INFO("  - BootInfo: ");
        FB_LOG_INFO_F("    - BootDriveNumber = 0x%02x", args.BootInfo.BootDriveNumber);
        FB_LOG_INFO_F("    - BootPartition = 0x%02x", args.BootInfo.BootPartition);
        FB_LOG_INFO("  - MemoryMap: ");
        FB_LOG_INFO_F("    - HasAcpiExtendedAttribute = %s", args.MemoryMap.HasAcpiExtendedAttribute ? "yes" : "no");
        FB_LOG_INFO_F("    - First = 0x%04X", args.MemoryMap.First);
        FB_LOG_INFO_F("    - EntriesCount = %d", args.MemoryMap.Count);
    }

    void PrintBootloaderParametersOption::HandleKey(HW::PS2::ScanCode code) {
        if (code == HW::PS2::ScanCode::Enter_Released) {
            SelectCurrentSubmenu(-1);
        }
    }

    void PrintBootDiskParameters::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Print bootable drive information");
    }

    void PrintBootDiskParameters::FetchState(String::StringBuffer& /*buffer*/) const {}

    void PrintBootDiskParameters::Enter() {
        Logging::GetTerminalManager()->ClearScreen();

        const auto& args = Bootloader::Get().GetBootloaderParameters();
        Driver::Drive::BiosDriveInterface interface(args.BootInfo.BootDriveNumber);

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

    void CPUIDInfo::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Print CPUID");
    }

    void CPUIDInfo::FetchState(String::StringBuffer& buffer) const {
        if (!HW::CPU::SupportsCpuid()) {
            String::Append(buffer, "NOT SUPPORTED");
        }
    }

    void CPUIDInfo::Enter() {
        if (!HW::CPU::SupportsCpuid()) {
            SelectCurrentSubmenu(-1);
            return;
        }
        Logging::GetTerminalManager()->ClearScreen();

        char string[256];
        String::StringBuffer buffer{string, 256};

        FB_LOG_INFO("CPUID");
        HW::CPU::GetVendorId(buffer);
        FB_LOG_INFO_F("  - Vendor ID: %s", string);

        string[0] = 0;
        if (HW::CPU::GetBrandString(buffer)) {
            FB_LOG_INFO_F("  - Brand string: %s", string);
        } else {
            FB_LOG_INFO("  - Brand string: unsupported");
        }

        FB_LOG_INFO_F("  - CpuidMaxFeature: 0x%08x", HW::CPU::GetCpuidMaxFeature());
        FB_LOG_INFO_F("  - CpuidMaxExtendedFeature: 0x%08x", HW::CPU::GetCpuidMaxExtendedFeature());

        string[0] = 0;
        HW::CPU::DecodeExtendedFeatureBits(HW::CPU::GetExtendedFeatureBits(), buffer);
        FB_LOG_INFO_F("  - CpuidExtendedFeatureBits: %s", string);
    }

    void CPUIDInfo::HandleKey(HW::PS2::ScanCode code) {
        if (code == HW::PS2::ScanCode::Enter_Released) {
            SelectCurrentSubmenu(-1);
        }
    }

    void QuitMenuOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Quit menu");
    }

    void QuitMenuOption::FetchState(String::StringBuffer& /*buffer*/) const {}

    void QuitMenuOption::Enter() {
        SelectCurrentSubmenu(-1);
        DebugMenu::HandleKey(HW::PS2::ScanCode::Escape_Released);
    }

    void QuitMenuOption::HandleKey(HW::PS2::ScanCode /*code*/) {}

    Memory::SizedBuffer<MenuOption*> GetMenuOptions() {
        static DebugModeOption c_debugModeOption{};
        static LogToSerial c_logToSerial{};
        static DebugDiskIOOption c_debugDiskIOOption{};
#ifdef F_DEBUG
        static DebugElfLoaderOption c_debugElfLoaderOption{};
#endif

        static PauseBeforeBootOption c_pauseBeforeBootOption{};
        static PrintMemoryMapOption c_printMemoryMapOption{};
        static PrintBootloaderParametersOption c_printBootloaderParametersOption{};
        static PrintBootDiskParameters c_printBootDiskParameters{};
        static CPUIDInfo c_cPUIDInfo{};
        static QuitMenuOption c_quitMenuOption{};

        static MenuOption* c_menuOptions[] = {
            &c_debugModeOption,
            &c_logToSerial,
            &c_debugDiskIOOption,
#ifdef F_DEBUG
            &c_debugElfLoaderOption,
#endif
            &c_pauseBeforeBootOption,
            &c_printMemoryMapOption,
            &c_printBootloaderParametersOption,
            &c_printBootDiskParameters,
            &c_cPUIDInfo,
            &c_quitMenuOption,
        };

        static Memory::SizedBuffer<MenuOption*> c_buffer = {c_menuOptions, F_SIZEOF_BUFFER(c_menuOptions)};
        return c_buffer;
    }
}  // namespace FunnyOS::Bootloader32::DebugMenu