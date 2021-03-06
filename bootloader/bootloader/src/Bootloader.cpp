#include "Bootloader.hpp"

#include <FunnyOS/Misc/TerminalManager/TerminalManagerLoggingSink.hpp>
#include <FunnyOS/Hardware/GFX/VGA.hpp>
#include <FunnyOS/Hardware/BIOS.hpp>
#include <FunnyOS/Hardware/CMOS.hpp>

#include "BiosFile.hpp"
#include "BootloaderConfig.hpp"
#include "HighMemory.hpp"

extern void* HEAP_START;

namespace FunnyOS::Bootloader64 {
    using namespace Stdlib;
    using namespace Misc;

    void Bootloader::Main(uint8_t bootDrive, uint8_t bootPartition, uintptr_t memoryTop) {
        // Init GDT
        m_bootloaderGDT[0] = 0;

        m_bootloaderGDT[GDT_SELECTOR_DATA64] = HW::CreateGdtDescriptor(
            {.BaseAddress    = 0x0000000,
             .Limit          = 0xFFFFFFF,
             .Type           = HW::GDTEntryType::Data,
             .GranularityBit = true,
             .IsPresent      = true});

        m_bootloaderGDT[GDT_SELECTOR_CODE64] = HW::CreateGdtDescriptor(
            {.BaseAddress              = 0x0000000,
             .Limit                    = 0xFFFFFFF,
             .Type                     = HW::GDTEntryType::Code,
             .GranularityBit           = true,
             .IsPresent                = true,
             .DescriptorPrivilegeLevel = 0,
             .IsConforming             = true,
             .IsLongMode               = true,
             .Is32Bit                  = false});

        m_bootloaderGDT[GDT_SELECTOR_DATA16] = HW::CreateGdtDescriptor(
            {.BaseAddress    = 0x0000000,
             .Limit          = 0xFFFFFFF,
             .Type           = HW::GDTEntryType::Data,
             .GranularityBit = true,
             .IsPresent      = true});

        m_bootloaderGDT[GDT_SELECTOR_CODE16] = HW::CreateGdtDescriptor(
            {.BaseAddress              = 0x0000000,
             .Limit                    = 0xFFFFFFF,
             .Type                     = HW::GDTEntryType::Code,
             .GranularityBit           = true,
             .IsPresent                = true,
             .DescriptorPrivilegeLevel = 0,
             .IsConforming             = true,
             .IsLongMode               = false,
             .Is32Bit                  = false});

        HW::LoadGdt({m_bootloaderGDT, F_SIZEOF_BUFFER(m_bootloaderGDT)});
        HW::LoadNewSegments(GDT_SELECTOR_CODE64, GDT_SELECTOR_DATA64);

        // Init allocator
        m_allocator.Initialize(reinterpret_cast<MemoryAllocator::memoryaddress_t>(&HEAP_START), memoryTop);

        // Setup BIOS calls
        HW::BIOS::SetupCallBiosInterface(
            GDT_SELECTOR_CODE64, GDT_SELECTOR_DATA64, GDT_SELECTOR_CODE16, GDT_SELECTOR_DATA16);

        // Init logging
        HW::BIOS::CallBios(0x10, "ah, al", 0x00, 0x03);  // 80x25 video mode

        auto vgaInterface = StaticRefCast<TerminalManager::ITerminalInterface>(MakeRef<HW::VGAInterface>());
        m_terminalManager = MakeRef<TerminalManager::TerminalManager>(vgaInterface);
        m_terminalManager->ClearScreen();

        m_logger.AddSink(MakeRef<TerminalManager::TerminalManagerLoggingSink>(m_terminalManager));
        FB_LOG_INFO("FunnyOS Bootloader");

        // Get RTC clock
        HW::CMOS::RTCTime time = HW::CMOS::FetchRTCTime();
        FB_LOG_INFO_F(
            "Date: %04d/%02d/%02d %02d:%02d", time.Year, time.Month, time.DayOfMonth, time.Hours, time.Minutes);

        // Initialize high memory
        InitializeHighMemory();
        DumpMemoryMap();

        // Memory stats
        FB_LOG_DEBUG_F("High memory available total:   %llu MB", GetTotalHighMemoryAvailable() / 1024 / 1024);
        FB_LOG_DEBUG_F(
            "High memory available current: %llu MB", GetHighMemoryAllocator().GetTotalAvailableMemory() / 1024 / 1024);

        // Init file system
        Bootparams::BootDriveInfo bootDriveInfo{bootDrive, bootPartition};
        InitializeFileSystem(bootDriveInfo);

        // Test file read
        Stdlib::Optional<Stdlib::File> file = Stdlib::OpenFile(F_BOOTLOADER_INI_FILE_PATH, Stdlib::FILE_OPEN_MODE_READ);

        if (!file) {
            FB_LOG_ERROR("no file :(");
        } else {
            // allocate string
            // read data
            Stdlib::Reader reader{Stdlib::Move(file->GetReadInterface())};
            Stdlib::Vector<uint8_t> vector = reader.ReadWhole();
            Stdlib::DynamicString string { reinterpret_cast<char*>(vector.Begin()), 0, vector.Size() };

            // print
            string.Replace("\n", "\r\n");
            FB_LOG_DEBUG_F("File: %s: \r\n%s", file->GetFileName().AsCString(), string.AsCString());
        }

        FB_LOG_OK("OK");

        for (;;) {
        }
    }

    Bootloader& Bootloader::Get() {
        static Bootloader c_bootloader;

        return c_bootloader;
    }

    Misc::MemoryAllocator::IMemoryAllocator& Bootloader::GetAllocator() {
        return m_allocator;
    }

    Stdlib::Logger& Bootloader::GetLogger() {
        return m_logger;
    }

    Misc::TerminalManager::TerminalManager& Bootloader::GetTerminalManager() {
        return *m_terminalManager.Get();
    }

}  // namespace FunnyOS::Bootloader64