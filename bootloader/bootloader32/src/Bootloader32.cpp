#include "Bootloader32.hpp"

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/BootloaderCommons/Logging.hpp>
#include <FunnyOS/BootloaderCommons/Sleep.hpp>
#include <FunnyOS/Hardware/CMOS.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include "Interrupts.hpp"
#include "A20Line.hpp"

#define _NO_RETURN for (;;)

namespace FunnyOS::Bootloader32 {
    using namespace FunnyOS::Stdlib;

    [[noreturn]] void Bootloader32Type::Main(const Bootloader::BootloaderParameters& args) {
        Bootloader::BootloaderType::Main(args);
        SetupInterrupts();
        Bootloader::SetupPIT();
        FB_LOG_INFO("FunnyOS Bootloader, hello!");

        if (!HW::PS2::InitializeKeyboard()) {
            FB_LOG_WARNING("Couldn't initialize keyboard");
        }

        // TODO: Move this
        FB_LOG_DEBUG("Printing memory map");
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

            FB_LOG_DEBUG_F(" at 0x%016llx - 0x%016llx b - %s", entry->BaseAddress, entry->Length, type);
        }

        const HW::CMOS::RTCTime time = HW::CMOS::FetchRTCTime();

        FB_LOG_INFO("Version: " FUNNYOS_VERSION);
        FB_LOG_INFO_F("Current date is: %04u/%02u/%02u %02u:%02u", time.Year, time.Month, time.DayOfMonth, time.Hours,
                      time.Minutes);
        FB_LOG_DEBUG("Debugging is enabled!");

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