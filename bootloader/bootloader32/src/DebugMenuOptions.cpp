#include "DebugMenuOptions.hpp"

#include <FunnyOS/Hardware/CPU.hpp>
#include "DebugMenu.hpp"
#include "DriveInterface.hpp"
#include "ElfLoader.hpp"
#include "FileLoader.hpp"
#include "Sleep.hpp"
#include "VESA.hpp"

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
        Logging::GetVgaOutputSink()->SetLevel(mode ? Stdlib::LogLevel::Debug : Stdlib::LogLevel::Info);
    }

    bool DebugModeOption::GetMode() const {
        return Logging::GetVgaOutputSink()->GetLevel() <= Stdlib::LogLevel::Debug;
    }

    void LogToSerial::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Log messages to COM1");
    }

    void LogToSerial::SetMode(bool mode) {
        Logging::GetSerialLoggingSink()->SetLevel(mode ? Stdlib::LogLevel::Debug : Stdlib::LogLevel::Off);
    }

    bool LogToSerial::GetMode() const {
        return Logging::GetSerialLoggingSink()->GetLevel() == Stdlib::LogLevel::Debug;
    }

    void DebugDiskIOOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Debug disk I/O");
    }

    void DebugDiskIOOption::SetMode(bool mode) {
        FileLoader::SetDebugReads(mode);

        if (mode) {
            Logging::GetVgaOutputSink()->SetLevel(Stdlib::LogLevel::Debug);
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
            Logging::GetVgaOutputSink()->SetLevel(Stdlib::LogLevel::Debug);
        }
    }

    bool DebugElfLoaderOption::GetMode() const {
        return ElfLoader::IsDebugElfs();
    }

    void IgnoreEdidOption::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Ignore EDID");
    }

    void IgnoreEdidOption::SetMode(bool mode) {
        SetIgnoreEdid(mode);
    }

    bool IgnoreEdidOption::GetMode() const {
        return IgnoreEdid();
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
            const auto& entry = memoryMap.First[i];

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
        DriveInterface interface(args.BootInfo.BootDriveNumber);

        FB_LOG_INFO("Boot drive parameters");
        FB_LOG_INFO_F("     - DriveIdentification = 0x%02x", interface.GetDriveIdentification());
        FB_LOG_INFO_F("     - TotalSectorCount = 0x%02x", interface.GetTotalSectorCount());
        FB_LOG_INFO_F("     - SectorSize = 0x%04x", interface.GetSectorSize());
        FB_LOG_INFO_F("     - HasExtendedDiskAccess = %s", interface.HasExtendedDiskAccess() ? "yes" : "no");
        FB_LOG_INFO_F(
            "     - HasEnhancedDiskDriveFunctions = %s", interface.HasEnhancedDiskDriveFunctions() ? "yes" : "no");
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

        char string[512];
        String::StringBuffer buffer{string, F_SIZEOF_BUFFER(string)};

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
        HW::CPU::DecodeFeatureBits(HW::CPU::GetFeatureBits(), buffer);
        FB_LOG_INFO_F("  - CpuidFeatureBits: %s", string);

        string[0] = 0;
        HW::CPU::DecodeExtendedFeatureBits(HW::CPU::GetExtendedFeatureBits(), buffer);
        FB_LOG_INFO_F("  - CpuidExtendedFeatureBits: %s", string);
    }

    void CPUIDInfo::HandleKey(HW::PS2::ScanCode code) {
        if (code == HW::PS2::ScanCode::Enter_Released) {
            SelectCurrentSubmenu(-1);
        }
    }

    void VESAInfo::FetchName(String::StringBuffer& buffer) const {
        String::Append(buffer, "Print VESA information");
    }

    void VESAInfo::FetchState(String::StringBuffer& /*buffer*/) const {}

    void VESAInfo::Enter() {
        m_currentVideoMode = 0;

        Logging::GetTerminalManager()->ClearScreen();

        FB_LOG_INFO("VESA information: ");
        const VbeInfoBlock& bloc = GetVbeInfoBlock();

        if (bloc.VbeVersion == 0) {
            FB_LOG_WARNING("VESA not supported");
            return;
        }

        FB_LOG_INFO_F("\tVbeVersion = 0x%04x", bloc.VbeVersion);
        FB_LOG_INFO_F("\tOemString = %s", static_cast<char*>(VesaPointerToVoidPointer(bloc.OemStringPtr)));
        FB_LOG_INFO_F("\tCapabilities = 0x%08x", bloc.Capabilities);
        FB_LOG_INFO_F("\tVideoModePtr = 0x%08x", bloc.VideoModePtr);
        FB_LOG_INFO_F("\tTotalMemory = 0x%04x", bloc.TotalMemory);
        FB_LOG_INFO_F("\tOemSoftwareRev = 0x%04x", bloc.OemSoftwareRev);
        FB_LOG_INFO_F("\tOemVendorName = %s", static_cast<char*>(VesaPointerToVoidPointer(bloc.OemVendorNamePtr)));
        FB_LOG_INFO_F("\tOemProductName = %s", static_cast<char*>(VesaPointerToVoidPointer(bloc.OemProductNamePtr)));
        FB_LOG_INFO_F("\tOemProductRev = %s", static_cast<char*>(VesaPointerToVoidPointer(bloc.OemProductRevPtr)));

        const auto& edid = GetEdidInformation();
        if (!edid) {
            FB_LOG_WARNING("\tEdidPreferredResolution: NO EDID");
        } else {
            uint32_t width;
            uint32_t height;

            for (int i = 0; i < 4; i++) {
                edid->FetchMaxResolution(i, width, height);
                FB_LOG_INFO_F("\tEdidPreferredResolution[%d]: %dx%d", i, width, height);
            }
        }

        FB_LOG_INFO_F("\tAvailable video modes: %d", GetVbeModes().Size);
        size_t validVideoModes = 0;
        for (const auto& mode : GetVbeModes()) {
            if (mode.IsValid) {
                validVideoModes++;
            }
        }

        FB_LOG_INFO_F("\tAvailable valid video modes: %zu", validVideoModes);
        FB_LOG_INFO("Use arrow keys to iterate over video modes");
    }

    void VESAInfo::HandleKey(HW::PS2::ScanCode code) {
        if (code == HW::PS2::ScanCode::Enter_Released) {
            SelectCurrentSubmenu(-1);
        } else if (code == HW::PS2::ScanCode::CursorDown_Released) {
            if (m_currentVideoMode + 1 < GetVbeModes().Size) {
                m_currentVideoMode++;
            }

            PrintVideoMode();
        } else if (code == HW::PS2::ScanCode::CursorUp_Released) {
            if (m_currentVideoMode > 0) {
                m_currentVideoMode--;
            }

            PrintVideoMode();
        }
    }

    void VESAInfo::PrintVideoMode() const {
        Logging::GetTerminalManager()->ClearScreen();

        const auto& mode = *GetVbeModes()[m_currentVideoMode];
        FB_LOG_INFO_F("Video mode %d: ", m_currentVideoMode);

        if (!mode.IsValid) {
            FB_LOG_WARNING("Invalid");
            return;
        }
        FB_LOG_INFO_F("\t ModeAttributes: 0b%016b", mode.ModeAttributes);
        FB_LOG_INFO_F("\t Window attributes: A = 0b%08b B = 0b%08b", mode.WindowA_Attributes, mode.WindowB_Attributes);
        FB_LOG_INFO_F("\t Window: granularity: %d KB, size = %d KB", mode.GranularityKB);
        FB_LOG_INFO_F(
            "\t Window Segment Start: A = 0x%08x B = 0x%08x", mode.WindowA_SegmentStart, mode.WindowB_SegmentStart);
        FB_LOG_INFO_F("\t WindowPositioningFunctionPtr: 0x%016x", mode.WindowPositioningFunctionPtr);
        FB_LOG_INFO_F("\t BytesPerScanline: %d", mode.BytesPerScanline);
        FB_LOG_INFO_F("\t Width: %d. Height: %d", mode.Width, mode.Height);
        FB_LOG_INFO_F("\t WidthOfCharacter: %d. HeightOfCharacter: %d", mode.WidthOfCharacter, mode.HeightOfCharacter);
        FB_LOG_INFO_F("\t NumberOfMemoryPlanes: %d", mode.NumberOfMemoryPlanes);
        FB_LOG_INFO_F("\t BitsPerPixel: %d", mode.BitsPerPixel);
        FB_LOG_INFO_F("\t NumberOfBanks: %d", mode.NumberOfBanks);
        FB_LOG_INFO_F("\t MemoryModel: %d", mode.MemoryModel);
        FB_LOG_INFO_F("\t BankSizeKb: %d", mode.BankSizeKb);
        FB_LOG_INFO_F("\t NumberOfImagePages: %d", mode.NumberOfImagePages);
        FB_LOG_INFO_F("\t Red: Mask = %d, Position = %d", mode.RedMask, mode.RedPosition);
        FB_LOG_INFO_F("\t Green: Mask = %d, Position = %d", mode.GreenMask, mode.GreenPosition);
        FB_LOG_INFO_F("\t Blue: Mask = %d, Position = %d", mode.BlueMask, mode.BluePosition);
        FB_LOG_INFO_F("\t Reserved: Mask = %d, Position = %d", mode.ReservedMask, mode.ReservedPosition);
        FB_LOG_INFO_F("\t DirectColorModeIfo: %d", mode.DirectColorModeIfo);
        FB_LOG_INFO_F("\t FrameBufferPhysicalAddress: 0x%016x", mode.FrameBufferPhysicalAddress);
        FB_LOG_INFO_F("\t OffscreenMemoryOffset: %d", mode.OffscreenMemoryOffset);
        FB_LOG_INFO_F("\t OffscreenMemorySize: %d", mode.OffscreenMemorySize);
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
        static IgnoreEdidOption c_ignoreEdidOption{};
        static PauseBeforeBootOption c_pauseBeforeBootOption{};
        static PrintMemoryMapOption c_printMemoryMapOption{};
        static PrintBootloaderParametersOption c_printBootloaderParametersOption{};
        static PrintBootDiskParameters c_printBootDiskParameters{};
        static CPUIDInfo c_CPUIDInfo{};
        static VESAInfo c_vesaInfo;
        static QuitMenuOption c_quitMenuOption{};

        static MenuOption* c_menuOptions[] = {
            &c_debugModeOption,
            &c_logToSerial,
            &c_debugDiskIOOption,
#ifdef F_DEBUG
            &c_debugElfLoaderOption,
#endif
            &c_ignoreEdidOption,
            &c_pauseBeforeBootOption,
            &c_printMemoryMapOption,
            &c_printBootloaderParametersOption,
            &c_printBootDiskParameters,
            &c_CPUIDInfo,
            &c_vesaInfo,
            &c_quitMenuOption,
        };

        static Memory::SizedBuffer<MenuOption*> c_buffer = {c_menuOptions, F_SIZEOF_BUFFER(c_menuOptions)};
        return c_buffer;
    }
}  // namespace FunnyOS::Bootloader32::DebugMenu