#include <FunnyOS/Kernel/Kernel.hpp>

#include <FunnyOS/Stdlib/Logging.hpp>
#include <FunnyOS/Hardware/GFX/FramebufferInterface.hpp>
#include <FunnyOS/Hardware/GFX/FontTerminalInterface.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManagerLoggingSink.hpp>
#include <FunnyOS/Kernel/GDT.hpp>

extern void* KERNEL_HEAP;
extern void* KERNEL_HEAP_TOP;

namespace FunnyOS::Kernel {
    Kernel64& Kernel64::Get() {
        static Kernel64 c_instance;
        return c_instance;
    }

    [[noreturn]] void Kernel64::Main(Bootparams::Parameters& parameters) {
        using namespace Misc::TerminalManager;

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

        // Setup temporary text logging in video mode
        const auto& videoMode = reinterpret_cast<Bootparams::VbeModeInfoBlock*>(
            parameters.Vbe.ModeInfoStart)[parameters.Vbe.ActiveModeIndex];

        auto framebufferInterface = MakeRef<HW::FramebufferInterface>(
            HW::FramebufferConfiguration{.Location = reinterpret_cast<void*>(videoMode.FrameBufferPhysicalAddress),
                                         .ScreenWidth = videoMode.Width,
                                         .ScreenHeight = videoMode.Height,
                                         .BPS = videoMode.BytesPerScanline,
                                         .BPP = static_cast<uint32_t>(videoMode.BitsPerPixel / 8),
                                         .RedPosition = videoMode.RedPosition,
                                         .GreenPosition = videoMode.GreenPosition,
                                         .BluePosition = videoMode.BluePosition});

        auto fontTerminalInterface = MakeRef<HW::FontTerminalInterface>(
            reinterpret_cast<uint8_t*>(m_parameters.BiosFonts), framebufferInterface);

        auto terminalManager = MakeRef<TerminalManager>(StaticRefCast<ITerminalInterface>(fontTerminalInterface));

        Stdlib::Logger logger;
        logger.AddSink(Stdlib::Ref<Stdlib::ILoggingSink>(new TerminalManagerLoggingSink(terminalManager)));

        F_LOG_DEBUG(logger, "We have kernel logger!");
        F_LOG_DEBUG_F(logger, "screen size in character %dx%d", fontTerminalInterface->GetScreenWidth(),
                      fontTerminalInterface->GetScreenHeight());
        F_LOG_DEBUG(logger, "Hello world from video mode :woah:");

        for (size_t i = 0; i < 35; i++) {
            F_LOG_WARNING_F(logger, "Hello %d", i);
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
