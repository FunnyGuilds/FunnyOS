#include "Bootloader32.hpp"

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/BootloaderCommons/Logging.hpp>
#include <FunnyOS/BootloaderCommons/Sleep.hpp>
#include <FunnyOS/Hardware/CMOS.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include "A20Line.hpp"
#include "DebugMenu.hpp"
#include "Interrupts.hpp"

#define _NO_RETURN for (;;)

// Defined in real_mode_intro.asm
extern const FunnyOS::Bootloader::BootloaderParameters bootloader_parameters;

namespace FunnyOS::Bootloader32 {
    using namespace FunnyOS::Stdlib;

    const FunnyOS::Bootloader::BootloaderParameters& Bootloader32Type::GetBootloaderParameters() {
        return bootloader_parameters;
    }

    [[noreturn]] void Bootloader32Type::Main() {
        Bootloader::BootloaderType::Main();
        SetupInterrupts();
        Bootloader::SetupPIT();
        FB_LOG_INFO("FunnyOS Bootloader, hello!");
        const HW::CMOS::RTCTime time = HW::CMOS::FetchRTCTime();

        FB_LOG_INFO("Version: " FUNNYOS_VERSION);
        FB_LOG_INFO_F("Current date is: %04u/%02u/%02u %02u:%02u", time.Year, time.Month, time.DayOfMonth, time.Hours,
                      time.Minutes);

        for (int i = 0; i < 3; i++) {
            if (HW::PS2::InitializeKeyboard()) {
                break;
            }

            FB_LOG_WARNING_F("Couldn't initialize keyboard, try %d/3", i + 1);
            Bootloader::Sleep(100);
        }

        // Debug menu
        Bootloader::Sleep(1000);
        if (DebugMenu::MenuRequested()) {
            DebugMenu::Enter();
        }

        if (Bootloader32::A20::IsEnabled()) {
            FB_LOG_DEBUG("A20 line is already enabled!");
        } else {
            Bootloader32::A20::TryEnable();

            if (!Bootloader32::A20::IsEnabled()) {
                FB_LOG_FATAL("No suitable method for enabling the A20 gate found. ");
                Halt();
            }

            FB_LOG_DEBUG("A20 enabled");
        }

        String::StringBuffer buf = Memory::AllocateBuffer<char>(10);
        for (size_t i = 0; i < 10; i++) {
            String::IntegerToString(buf, i);
            FB_LOG_INFO(buf.Data);
            Bootloader::Sleep(1000);
        }

        Halt();
        _NO_RETURN;
    }

    [[noreturn]] void Bootloader32Type::Panic(const char* details) {
        void* cause = F_FETCH_CALLER_ADDRESS();
        char callerAddress[17];
        Memory::SizedBuffer<char> callerAddressBuffer{static_cast<char*>(callerAddress), 17};
        String::IntegerToHex(callerAddressBuffer, reinterpret_cast<uintptr_t>(cause));

        using namespace FunnyOS::Misc::TerminalManager;

        TerminalManager* terminal = Bootloader::Logging::GetTerminalManager();
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

    [[noreturn]] void Bootloader32Type::Halt() {
        //        HW::DisableHardwareInterrupts(); TODO

        for (;;) {
            HW::CPU::Halt();
        }
    }

}  // namespace FunnyOS::Bootloader32