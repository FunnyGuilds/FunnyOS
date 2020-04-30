#include <FunnyOS/Kernel/Kernel.hpp>

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Hardware/VGA.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>

namespace FunnyOS::Kernel {

    Kernel64& Kernel64::Get() {
        static Kernel64 c_instance;
        return c_instance;
    }

    void Kernel64::Initialize(Bootparams::Parameters& parameters) {
        m_parameters = parameters;

        using namespace Misc::TerminalManager;

        HW::VGAInterface vga;
        TerminalManager terminalManager(&vga);

        terminalManager.ClearScreen();
        terminalManager.ChangeForegroundColor(Color::LightGreen);
        terminalManager.PrintLine("Hello from kernel :woah:");

        // Print some test values to make sure we passed the boot params properly
        uint64_t stack;
        asm("mov %0, rsp" : "=g"(stack));

        char testBuf[256] = {0};
        Stdlib::Memory::SizedBuffer<char> buf{testBuf, F_SIZEOF_BUFFER(testBuf)};
        Stdlib::String::Format(buf,
                               "Parameters location: 0x%016llx. \r\n"
                               "\tDrive: 0x%02x.\r\n"
                               "\tMemory map: 0x%08x.\r\n\r\n"
                               "Current stack: %016llx",
                               &parameters, m_parameters.BootInfo.BootDriveNumber, m_parameters.MemoryMap.First, stack);

        terminalManager.PrintLine(testBuf);

        for (;;)
            ;
    }

    const Bootparams::Parameters& Kernel64::GetParameters() const {
        return m_parameters;
    }

    Kernel64::Kernel64() = default;

}  // namespace FunnyOS::Kernel