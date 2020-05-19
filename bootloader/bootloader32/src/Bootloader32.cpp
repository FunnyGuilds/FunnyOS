#include "Bootloader32.hpp"

#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Hardware/CMOS.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Hardware/PIC.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include <FunnyOS/Hardware/GFX/VGA.hpp>
#include "A20Line.hpp"
#include "DebugMenu.hpp"
#include "DriveInterface.hpp"
#include "ElfLoader.hpp"
#include "Interrupts.hpp"
#include "Logging.hpp"
#include "MemoryMap.hpp"
#include "Paging.hpp"
#include "RealMode.hpp"
#include "Sleep.hpp"
#include "VESA.hpp"

// Defined in start.asm
extern FunnyOS::Bootparams::BootDriveInfo g_bootInfo;

// Linker symbols
extern void* HEAP_START;

namespace FunnyOS::Bootloader32 {
    using namespace FunnyOS::Stdlib;

    constexpr const size_t BIOS_FONTS_SIZE = 256 * 16;

    Bootparams::Parameters& Bootloader::GetBootloaderParameters() {
        static Bootparams::Parameters c_bootloaderParameters;
        return c_bootloaderParameters;
    }

    uint8_t* FetchBiosFonts() {
        auto biosFonts = Memory::AllocateBuffer<uint8_t>(BIOS_FONTS_SIZE);
        Registers32 registers16;
        registers16.EAX.Value16 = 0x1130;
        registers16.EBX.Value8.High = 0x06;
        RealModeInt(0x10, registers16);

        const auto* fontsLocation = reinterpret_cast<uint8_t*>(registers16.ES.Value16 * 16 + registers16.EBP.Value16);
        Memory::Copy<uint8_t>(biosFonts, fontsLocation);

        return biosFonts.Data;
    }

    [[noreturn]] void Bootloader::Main() {
        GetBootloaderParameters().BootInfo = g_bootInfo;
        int error = CreateMemoryMap(GetBootloaderParameters().MemoryMap);
        if (error != 0) {
            Halt();
        }

        // Initialization stuff
        GetAllocator().Initialize(reinterpret_cast<Misc::MemoryAllocator::memoryaddress_t>(&HEAP_START), 0x00080000);
        SetupInterrupts();
        SetupPIT();
        Logging::InitLogging();

        FB_LOG_INFO("FunnyOS Bootloader, hello!");
        const HW::CMOS::RTCTime time = HW::CMOS::FetchRTCTime();

        FB_LOG_INFO("Version: " FUNNYOS_VERSION);
        FB_LOG_INFO_F("Current date is: %04u/%02u/%02u %02u:%02u", time.Year, time.Month, time.DayOfMonth, time.Hours,
                      time.Minutes);

        for (int i = 0; i < 3; i++) {
            if (HW::PS2::InitializeKeyboard()) {
                FB_LOG_OK("Keyboard initialized!");
                break;
            }

            FB_LOG_WARNING_F("Couldn't initialize keyboard, try %d/3", i + 1);
            Sleep(100);
        }

        // Debug menu
        Sleep(1000);
        if (DebugMenu::MenuRequested()) {
            DebugMenu::Enter();
        }

        if (A20::IsEnabled()) {
            FB_LOG_DEBUG("A20 line is already enabled!");
        } else {
            A20::TryEnable();

            if (!A20::IsEnabled()) {
                FB_LOG_FATAL("No suitable method for enabling the A20 gate found. ");
                Halt();
            }

            FB_LOG_OK("A20 line enabled!");
        }

        if (!HW::CPU::SupportsCpuid()) {
            FB_LOG_FATAL("CPUID instruction is not supported, booting failed.");
            Halt();
        }

        if ((HW::CPU::GetExtendedFeatureBits() & static_cast<uint64_t>(HW::CPU::CPUIDExtendedFeatures::LM)) == 0) {
            FB_LOG_FATAL("Long mode is not supported, booting failed.");
            Halt();
        }

        char vendorId[13];
        String::StringBuffer vendorIdBuffer{vendorId, 13};
        HW::CPU::GetVendorId(vendorIdBuffer);
        FB_LOG_INFO_F("CPU Vendor ID: %s", vendorId);

        // Prepare VBE information and find best video mode
        GetBootloaderParameters().Vbe.EdidBlock =
            GetEdidInformation().Map<Bootparams::Pointer32<EdidInformation>>([](auto& block) { return &block; });

        GetBootloaderParameters().Vbe.InfoBlock = &GetVbeInfoBlock();
        GetBootloaderParameters().Vbe.ModeInfoStart = GetVbeModes().Data;
        GetBootloaderParameters().Vbe.ModeInfoEntries = static_cast<uint16_t>(GetVbeModes().Size);
        Optional<uint16_t> bestVideoMode = PickBestMode();

        if (!bestVideoMode) {
            FB_LOG_FATAL("Failed to find a suitable video mode");
            Halt();
        }

        GetBootloaderParameters().Vbe.ActiveModeIndex = bestVideoMode.GetValue();
        const auto& activeMode = *GetVbeModes()[bestVideoMode.GetValue()];
        FB_LOG_INFO_F("Will use %dx%d video mode", activeMode.Width, activeMode.Height);

        // Find biggest available memory segment
        const size_t minimumSize = 1024 * 1024;  // 16 MB // TODO: calculate the actual size?
        auto kernelMem = FindBiggestUsableMemoryEntry(GetBootloaderParameters().MemoryMap);

        if (kernelMem.Type != Bootparams::MemoryMapEntryType::AvailableMemory) {
            FB_LOG_FATAL("Could not allocate ANY memory to load the kernel.");
            Halt();
        }

        if (kernelMem.Length < minimumSize) {
            FB_LOG_FATAL_F(
                "Could not allocate enough memory to load kernel (allocated %llu bytes out of %llu required)",
                kernelMem.Length, minimumSize);
        }

        // Some debug info
        FB_LOG_DEBUG_F("Will load kernel at 0x%08llx, size is %llu bytes (%llu MB)", kernelMem.BaseAddress,
                       kernelMem.Length, kernelMem.Length / 1024ULL / 1024ULL);

        // Get ourselves an allocator for that memory hole
        Misc::MemoryAllocator::StaticMemoryAllocator highMemoryAllocator{};
        highMemoryAllocator.Initialize(kernelMem.BaseAddress, kernelMem.BaseAddress + kernelMem.Length - 1);

        // Prepare drive interface and file loader
        DriveInterface driveInterface(GetBootloaderParameters().BootInfo.BootDriveNumber);
        FileLoader bootPartitionFileLoader(driveInterface, GetBootloaderParameters().BootInfo.BootPartition);

        // Load env 64
        ElfLoader lowMemoryElfLoader(GetAllocator(), bootPartitionFileLoader);
        void* env64 = lowMemoryElfLoader.LoadRegularFile("/boot/env64").Data;
        FB_LOG_DEBUG_F("env64 loaded at %08x", env64);

        // Load raw kernel elf to memory
        ElfLoader highMemoryElfLoader(highMemoryAllocator, bootPartitionFileLoader);
        void* kernelRawElf = highMemoryElfLoader.LoadRegularFile("/system/fkrnl.fxe").Data;
        FB_LOG_DEBUG_F("fkrnl.fxe loaded at %08x as raw file", kernelRawElf);

        // Load kernel .elf
        ElfFileInfo kernelElf = highMemoryElfLoader.LoadElfFile(kernelRawElf);
        FB_LOG_DEBUG_F("Kernel physical memory location: 0x%08llx, size: %llu bytes", kernelElf.PhysicalLocationBase,
                       kernelElf.TotalMemorySize);

        if (kernelElf.VirtualLocationBase != GetKernelVirtualLocation()) {
            FB_LOG_ERROR_F("Kernel virtual location differs from expected. Expected: %016llx. Actual: %016llx",
                           GetKernelVirtualLocation(), kernelElf.VirtualLocationBase);
            Halt();
        }

        // Setup pages
        Misc::MemoryAllocator::StaticMemoryAllocator pageTableMemoryAllocator{};
        const uintmax_t pageTableMemoryLocation = highMemoryAllocator.GetCurrentMemoryTop();
        pageTableMemoryAllocator.Initialize(pageTableMemoryLocation, highMemoryAllocator.GetMemoryEnd());

        SimplePageTableAllocator pageTableAllocator(pageTableMemoryAllocator);

        // Identity map first two megabytes
        pageTableAllocator.Map2MbPage(0, 0);

        // Map kernel
        pageTableAllocator.MapLocation(kernelElf.PhysicalLocationBase, kernelElf.TotalMemorySize,
                                       GetKernelVirtualLocation());

        // Map physical memory, first 4 GB
        constexpr const uint64_t PHYSICAL_MEMORY_MAP_SIZE = 1024ULL * 1024ULL * 1024ULL * 4ULL;  // 4 GB

        for (uint64_t i = 0; i < (PHYSICAL_MEMORY_MAP_SIZE / PAGE_DIRECTORY_ENTRY_SIZE); i++) {
            const uint64_t offset = i * PAGE_DIRECTORY_ENTRY_SIZE;
            pageTableAllocator.Map2MbPage(offset, GetPhysicalMemoryVirtualLocation() + offset);
        }

        // Debug info
        const uintmax_t pageTableMemorySize = pageTableMemoryAllocator.GetCurrentMemoryTop() - pageTableMemoryLocation;

        FB_LOG_DEBUG_F("Temporary page table memory location: 0x%08x, size: %u bytes", pageTableMemoryLocation,
                       pageTableMemorySize);

        // Add new entries to memory map
        auto& memoryMap = GetBootloaderParameters().MemoryMap;
        Bootparams::MemoryMapEntry* memoryMapBase = memoryMap.First;

        // Kernel image entry
        memoryMapBase[memoryMap.Count] = {.BaseAddress = kernelElf.PhysicalLocationBase,
                                          .Length = kernelElf.TotalMemorySize,
                                          .Type = Bootparams::MemoryMapEntryType::KernelImage,
                                          .ACPIFlags = 0b01};
        memoryMap.Count++;

        // Page tables entry
        memoryMapBase[memoryMap.Count] = {.BaseAddress = pageTableMemoryLocation,
                                          .Length = pageTableMemorySize,
                                          .Type = Bootparams::MemoryMapEntryType::PageTableReclaimable,
                                          .ACPIFlags = 0b01};
        memoryMap.Count++;

        // Treat first megabyte as reserved
        memoryMapBase[memoryMap.Count] = {.BaseAddress = 0,
                                          .Length = 0x100000,
                                          .Type = Bootparams::MemoryMapEntryType::ReservedMemory,
                                          .ACPIFlags = 0b01};
        memoryMap.Count++;

        // Fetch bios fonts
        GetBootloaderParameters().BiosFonts = FetchBiosFonts();
        GetBootloaderParameters().BiosFontsSize = BIOS_FONTS_SIZE;
        FB_LOG_DEBUG_F("Bios fonts at %08x", GetBootloaderParameters().BiosFonts);

        // Pause if necessary
        if (IsPauseBeforeBoot()) {
            FB_LOG_INFO("To boot press enter ...");
            Pause();
        }

        // Setup video mode
        FB_LOG_OK("Setting up video mode");
        SelectVideoMode(GetBootloaderParameters().Vbe.ActiveModeIndex);

        FB_LOG_OK("Booting...");

        // Disable all interrupts
        HW::DisableHardwareInterrupts();
        HW::DisableNonMaskableInterrupts();
        HW::PIC::SetEnabledInterrupts(0);

        // Jump to env64
        auto kernelEntryPointLow = static_cast<uint32_t>(kernelElf.EntryPointVirtual & 0xFFFFFFFF);
        auto kernelEntryPointHigh = static_cast<uint32_t>(kernelElf.EntryPointVirtual >> 32);

        auto bootParamsAddressLow = reinterpret_cast<uint32_t>(&GetBootloaderParameters());

        asm("push $0\n"  // Boot params address high
            "push %3\n"  // Boot params address low
            "push %2\n"  // Kernel entry point high
            "push %1\n"  // Kernel entry point low
            "push %0\n"  // PML4 base
            "jmp *%%ebx\n"
            :
            : "r"(pageTableAllocator.GetPml4Base()), "r"(kernelEntryPointLow), "r"(kernelEntryPointHigh),
              "r"(bootParamsAddressLow), "b"(env64));

        F_NO_RETURN;
    }

    [[noreturn]] void Bootloader::Panic(const char* details) {
        void* cause = F_FETCH_CALLER_ADDRESS();
        char callerAddress[17];
        Memory::SizedBuffer<char> callerAddressBuffer{static_cast<char*>(callerAddress), 17};
        String::IntegerToHex(callerAddressBuffer, reinterpret_cast<uintptr_t>(cause));

        using namespace FunnyOS::Misc::TerminalManager;

        auto& terminal = Logging::GetTerminalManager();
        terminal->ChangeColor(Color::Red, Color::White);
        terminal->ClearScreen();
        terminal->PrintString("================================================================================");
        terminal->PrintString("                             Bootloader panic ! >:(                             ");
        terminal->PrintString("================================================================================");
        terminal->PrintString("\nDetails: \r\n    ");
        terminal->PrintLine(details);
        terminal->PrintString("\nCaller address: \r\n    0x");
        terminal->PrintLine(static_cast<const char*>(callerAddress));

        Halt();
        F_NO_RETURN;
    }

    [[noreturn]] void Bootloader::Halt() {
        HW::DisableHardwareInterrupts();

        for (;;) {
            HW::CPU::Halt();
        }
    }

    Misc::MemoryAllocator::StaticMemoryAllocator& Bootloader::GetAllocator() {
        return m_allocator;
    }

    Bootloader& Bootloader::Get() {
        static FunnyOS::Bootloader32::Bootloader c_bootloader;
        return c_bootloader;
    }

    bool Bootloader::IsPauseBeforeBoot() const {
        return m_pauseBeforeBoot;
    }

    void Bootloader::SetPauseBeforeBoot(bool pauseBeforeBoot) {
        m_pauseBeforeBoot = pauseBeforeBoot;
    }
}  // namespace FunnyOS::Bootloader32