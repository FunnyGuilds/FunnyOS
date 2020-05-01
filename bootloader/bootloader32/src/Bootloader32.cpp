#include "Bootloader32.hpp"

#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Hardware/CMOS.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Hardware/PIC.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include "A20Line.hpp"
#include "DebugMenu.hpp"
#include "DriveInterface.hpp"
#include "ElfLoader.hpp"
#include "Interrupts.hpp"
#include "Logging.hpp"
#include "Paging.hpp"
#include "Sleep.hpp"

// Defined in real_mode_intro.asm
extern FunnyOS::Bootparams::BootDriveInfo g_bootInfo;
extern FunnyOS::Bootparams::MemoryMapDescription g_memoryMap;

// Linker symbols
extern void* REAL_CODE_START;
extern void* REAL_BUFFER_START;
extern void* REAL_BUFFER_END;

namespace FunnyOS::Bootloader32 {
    using namespace FunnyOS::Stdlib;

    Bootparams::Parameters& Bootloader::GetBootloaderParameters() {
        static Bootparams::Parameters c_bootloaderParameters;
        return c_bootloaderParameters;
    }

    namespace {
        Bootparams::MemoryMapEntry FindBiggestUsableEntry(const Bootparams::MemoryMapDescription& memoryMap) {
            constexpr const uint64_t MINIMUM_ADDRESSABLE_BYTE = 1024ULL * 1024ULL * 2ULL;  // 2 MB
            constexpr const uint64_t MAXIMUM_ADDRESSABLE_BYTE = NumeralTraits::Info<uint32_t>::MaximumValue;

            Bootparams::MemoryMapEntry biggestEntry{0, 0, Bootparams::MemoryMapEntryType::Reserved, 0};

            for (size_t i = 0; i < memoryMap.Count; i++) {
                const auto& entry = memoryMap[i];

                if (entry.Type != Bootparams::MemoryMapEntryType::AvailableMemory) {
                    continue;
                }

                if (memoryMap.HasAcpiExtendedAttribute) {
                    if ((entry.ACPIFlags & Bootparams::ACPI30Flags::DONT_IGNORE) == 0) {
                        continue;
                    }

                    if ((entry.ACPIFlags & Bootparams::ACPI30Flags::MEMORY_VOLATILE) != 0) {
                        continue;
                    }
                }

                if (entry.BaseAddress > MAXIMUM_ADDRESSABLE_BYTE) {
                    continue;
                }

                // Found suitable memory, now check for size and boundaries
                uint64_t entryMinimumByte = entry.BaseAddress;
                uint64_t entryMaximumByte = entry.BaseAddress + entry.Length - 1;

                // Base address must be page-aligned
                if ((entryMinimumByte % 0x1000) != 0) {
                    entryMinimumByte += 0x1000 - (entryMinimumByte % 0x1000);
                }

                // We cannot use addresses larger than 32-bit in 32-bit mode.
                if (entryMaximumByte > MAXIMUM_ADDRESSABLE_BYTE) {
                    entryMaximumByte = MAXIMUM_ADDRESSABLE_BYTE;
                }

                // We cannot use addresses < than 2 MB since they will be identity mapped
                if (entryMinimumByte < MINIMUM_ADDRESSABLE_BYTE) {
                    entryMinimumByte = MINIMUM_ADDRESSABLE_BYTE;
                }

                if (entryMinimumByte > entryMaximumByte) {
                    // Whole entry is below minimum or above maximum
                    continue;
                }

                const uint64_t totalUsableSpace = entryMaximumByte - entryMinimumByte + 1;

                // Found suitable memory area
                if (biggestEntry.Length < totalUsableSpace) {
                    biggestEntry.BaseAddress = entryMinimumByte;
                    biggestEntry.Length = totalUsableSpace;
                    biggestEntry.Type = entry.Type;
                    biggestEntry.ACPIFlags = entry.ACPIFlags;
                }
            }

            return biggestEntry;
        }
    }  // namespace

    [[noreturn]] void Bootloader::Main() {
        GetBootloaderParameters().BootInfo = g_bootInfo;
        GetBootloaderParameters().MemoryMap = g_memoryMap;

        // Initialization stuff
        GetAllocator().Initialize(0x00040000, 0x0007FFFF);
        SetupInterrupts();
        SetupPIT();
        Logging::InitSerialLogging();

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

        // Find biggest available memory segment
        const size_t minimumSize = 1024 * 1024;  // 16 MB // TODO: calculate the actual size?
        auto kernelMem = FindBiggestUsableEntry(GetBootloaderParameters().MemoryMap);

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
        void* env64 = lowMemoryElfLoader.LoadRegularFile("/boot/env64");
        FB_LOG_DEBUG_F("env64 loaded at %08x", env64);

        // Load raw kernel elf to memory
        ElfLoader highMemoryElfLoader(highMemoryAllocator, bootPartitionFileLoader);
        void* kernelRawElf = highMemoryElfLoader.LoadRegularFile("/system/fkrnl.fxe");
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
        const uintmax_t pageTableMemoryLocation = AlignToPage(highMemoryAllocator.GetCurrentMemoryTop());
        pageTableMemoryAllocator.Initialize(pageTableMemoryLocation, highMemoryAllocator.GetMemoryEnd());

        void* pml4base = SetupInitialKernelPages(kernelElf.PhysicalLocationBase, kernelElf.TotalMemorySize,
                                                 pageTableMemoryAllocator);

        const uintmax_t pageTableMemorySize = pageTableMemoryAllocator.GetCurrentMemoryTop() - pageTableMemoryLocation;

        FB_LOG_DEBUG_F("Temporary page table physical memory location: 0x%08x, size: %u bytes", pageTableMemoryLocation,
                       pageTableMemorySize);

        // Add new entries to memory map
        auto& memoryMap = GetBootloaderParameters().MemoryMap;
        auto* memoryMapBase = reinterpret_cast<Bootparams::MemoryMapEntry*>(memoryMap.First);

        // Kernel image entry
        memoryMapBase[memoryMap.Count] = {
            .BaseAddress = kernelElf.PhysicalLocationBase,
            .Length = kernelElf.TotalMemorySize,
            .Type = Bootparams::MemoryMapEntryType::KernelImage,
            .ACPIFlags = static_cast<uint32_t>(memoryMap.HasAcpiExtendedAttribute ? 0b01 : 0)};
        memoryMap.Count++;

        // Page tables entry
        memoryMapBase[memoryMap.Count] = {
            .BaseAddress = pageTableMemoryLocation,
            .Length = pageTableMemorySize,
            .Type = Bootparams::MemoryMapEntryType::PageTableReclaimable,
            .ACPIFlags = static_cast<uint32_t>(memoryMap.HasAcpiExtendedAttribute ? 0b01 : 0)};
        memoryMap.Count++;

        // Pause if necessary
        if (IsPauseBeforeBoot()) {
            FB_LOG_INFO("To boot press enter ...");
            Pause();
        }

        FB_LOG_OK("Booting...");

        // Disable all interrupts hardware interrupts..
        HW::DisableHardwareInterrupts();
        HW::PIC::SetEnabledInterrupts(0);

        // Jump to env64
        auto kernelEntryPointLow = static_cast<uint32_t>(kernelElf.EntryPointVirtual & 0xFFFFFFFF);
        auto kernelEntryPointHigh = static_cast<uint32_t>(kernelElf.EntryPointVirtual >> 32);

        auto bootParamsAddressLow = reinterpret_cast<uint32_t>(&GetBootloaderParameters());

        asm("push 0\n"   // Boot params address high
            "push %3\n"  // Boot params address low
            "push %2\n"  // Kernel entry point high
            "push %1\n"  // Kernel entry point low
            "push %0\n"  // PML4 base
            "jmp ebx\n"
            :
            : "r"(pml4base), "r"(kernelEntryPointLow), "r"(kernelEntryPointHigh), "r"(bootParamsAddressLow),
              "b"(env64));

        F_NO_RETURN;
    }

    [[noreturn]] void Bootloader::Panic(const char* details) {
        void* cause = F_FETCH_CALLER_ADDRESS();
        char callerAddress[17];
        Memory::SizedBuffer<char> callerAddressBuffer{static_cast<char*>(callerAddress), 17};
        String::IntegerToHex(callerAddressBuffer, reinterpret_cast<uintptr_t>(cause));

        using namespace FunnyOS::Misc::TerminalManager;

        TerminalManager* terminal = Logging::GetTerminalManager();
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