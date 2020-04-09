#include "Bootloader32.hpp"

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/BootloaderCommons/Logging.hpp>
#include "Interrupts.hpp"
#include "A20Line.hpp"

#define _NO_RETURN for (;;)

namespace FunnyOS::Bootloader32 {
    using namespace FunnyOS::Stdlib;

    [[noreturn]] void Bootloader32Type::Main(const Bootloader::BootloaderParameters& args) {
        Bootloader::BootloaderType::Main(args);
        SetupInterrupts();

        __asm__ __volatile__ ("int 0x69");

        if (Bootloader32::A20::IsEnabled()) {
            FB_LOG_INFO("A20 line is already enabled!");
        } else {
            // TODO: Actually enable the line
        }

        FB_LOG_DEBUG("this is a debug message");
        FB_LOG_INFO("this is an info message");
        FB_LOG_OK("this is an ok message");
        FB_LOG_WARNING("this is a waring message");
        FB_LOG_ERROR("this is an error message");
        FB_LOG_FATAL("this is a fatal message");

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
        for (;;) {
#ifdef __GNUC__
            __asm__ __volatile__(
                "cli \n"
                "hlt");
#endif
        }
    }

}  // namespace FunnyOS::Bootloader32