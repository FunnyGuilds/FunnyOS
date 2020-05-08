#include <FunnyOS/Kernel/Kernel.hpp>

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Hardware/GFX/FramebufferInterface.hpp>
#include <FunnyOS/Hardware/GFX/FontTerminalInterface.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>
#include <FunnyOS/Kernel/GDT.hpp>

extern void* KERNEL_HEAP;
extern void* KERNEL_HEAP_TOP;

namespace FunnyOS::Kernel {
    Kernel64& Kernel64::Get() {
        static Kernel64 c_instance;
        return c_instance;
    }

    [[noreturn]] void Kernel64::Main(Bootparams::Parameters& parameters) {
        if (m_initialized) {
            // TODO Panic();
        }

        m_initialized = true;
        m_parameters = parameters;

        // Initialize allocator
        using Misc::MemoryAllocator::memoryaddress_t;
        m_kernelAllocator.Initialize(reinterpret_cast<memoryaddress_t>(&KERNEL_HEAP),
                                     reinterpret_cast<memoryaddress_t>(&KERNEL_HEAP_TOP));

        // Load kernel GDT
        LoadKernelGdt();
        LoadNewSegments(GDT_SELECTOR_CODE_RING0, GDT_SELECTOR_DATA);

        const auto& videoMode = reinterpret_cast<Bootparams::VbeModeInfoBlock*>(
            parameters.Vbe.ModeInfoStart)[parameters.Vbe.ActiveModeIndex];

        // Setup framebuffer interface
        HW::FramebufferInterface framebufferInterface(
            {.Location = reinterpret_cast<void*>(videoMode.FrameBufferPhysicalAddress),
             .ScreenWidth = videoMode.Width,
             .ScreenHeight = videoMode.Height,
             .BPS = videoMode.BytesPerScanline,
             .BPP = static_cast<uint32_t>(videoMode.BitsPerPixel / 8),
             .RedPosition = videoMode.RedPosition,
             .GreenPosition = videoMode.GreenPosition,
             .BluePosition = videoMode.BluePosition});

        HW::FontTerminalInterface fontTerminalInterface(reinterpret_cast<uint8_t*>(m_parameters.BiosFonts),
                                                        &framebufferInterface);

        Misc::TerminalManager::TerminalManager manager(&fontTerminalInterface);

        String::StringBuffer stringBuffer = Memory::AllocateBuffer<char>(128);
        String::Format(stringBuffer, "screen size in character %dx%d", fontTerminalInterface.GetScreenWidth(),
                       fontTerminalInterface.GetScreenHeight());

        manager.PrintLine(stringBuffer.Data);
        manager.PrintLine("Hello world from video mode :woah:");

        for (size_t i = 0; i < 4096; i++) {
            String::Format(stringBuffer, "hello %d", i);
            manager.PrintLine(stringBuffer.Data);
        }

        for (;;) {
            asm volatile(
                "cli\n"
                "hlt");
        }
    }

    const Bootparams::Parameters& Kernel64::GetParameters() const {
        return m_parameters;
    }

    VMM::VirtualMemoryManager& Kernel64::GetVirtualMemoryManager() {
        return m_virtualMemoryManager;
    }

    Misc::MemoryAllocator::StaticMemoryAllocator& Kernel64::GetKernelAllocator() {
        return m_kernelAllocator;
    }

    Kernel64::Kernel64() = default;

}  // namespace FunnyOS::Kernel
