#include "Bootloader32.hpp"

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Hardware/CMOS.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include <FunnyOS/Hardware/RealMode.hpp>

#include "A20Line.hpp"
#include "DebugMenu.hpp"
#include "Interrupts.hpp"
#include "Logging.hpp"
#include "Sleep.hpp"

#define _NO_RETURN for (;;)

// Defined in real_mode_intro.asm
extern FunnyOS::Bootparams::BootDriveInfo g_bootInfo;
extern FunnyOS::Bootparams::MemoryMapDescription g_memoryMap;

namespace FunnyOS::Bootloader32 {
    using namespace FunnyOS::Stdlib;

    Bootparams::Parameters& Bootloader::GetBootloaderParameters() {
        static Bootparams::Parameters c_bootloaderParameters;
        return c_bootloaderParameters;
    }

    [[noreturn]] void Bootloader::Main() {
        GetBootloaderParameters().BootInfo = g_bootInfo;
        GetBootloaderParameters().MemoryMap = g_memoryMap;

        // Initialization stuff
        GetAllocator().Initialize(0x00040000, 0x0007FFFF);
        SetupInterrupts();
        SetupPIT();

        HW::SetupRealModeInterrupts({
            .SelectorCode32 = 0x08 * 1,
            .SelectorData32 = 0x08 * 2,
            .SelectorCode16 = 0x08 * 3,
            .SelectorData16 = 0x08 * 4,
        });

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

        Halt();
        _NO_RETURN;
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
        _NO_RETURN;
    }

    [[noreturn]] void Bootloader::Halt() {
        HW::DisableHardwareInterrupts();

        for (;;) {
            HW::CPU::Halt();
        }
    }

    LowMemoryAllocator& Bootloader::GetAllocator() {
        return m_allocator;
    }

    Bootloader& Bootloader::Get() {
        static FunnyOS::Bootloader32::Bootloader c_bootloader;
        return c_bootloader;
    }
}  // namespace FunnyOS::Bootloader32