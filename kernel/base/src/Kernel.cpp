#include <FunnyOS/Kernel/Kernel.hpp>

#include <FunnyOS/Stdlib/Logging.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
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

        // Setup boot parameters
        m_bootDriveInfo = parameters.BootInfo;

        // Initialize allocator
        using Misc::MemoryAllocator::memoryaddress_t;
        m_kernelAllocator.Initialize(reinterpret_cast<memoryaddress_t>(&KERNEL_HEAP),
                                     reinterpret_cast<memoryaddress_t>(&KERNEL_HEAP_TOP));

        // Load kernel GDT
        LoadKernelGdt();
        LoadNewSegments(GDT_SELECTOR_CODE_RING0, GDT_SELECTOR_DATA);

        // Setup basic screen manager

        // Copy video modes
        Stdlib::Vector<Bootparams::VbeModeInfoBlock> videoModes;
        videoModes.EnsureCapacity(parameters.Vbe.ModeInfoEntries);
        for (size_t i = 0; i < parameters.Vbe.ModeInfoEntries; i++) {
            videoModes.Append(parameters.Vbe.ModeInfoStart[i]);
        }
        videoModes.ShrinkToSize();

        // Copy edid
        Stdlib::Optional<Bootparams::EdidInformation> edid =
            parameters.Vbe.EdidBlock.Map<Bootparams::EdidInformation>([](const auto& ptr) { return *ptr; });

        // Copy fonts
        auto fonts = Stdlib::Memory::AllocateBuffer<uint8_t>(parameters.BiosFontsSize);
        Stdlib::Memory::Copy(fonts, static_cast<uint8_t*>(parameters.BiosFonts));

        const auto& videoMode = parameters.Vbe.ModeInfoStart[parameters.Vbe.ActiveModeIndex];

        auto framebufferConfig =
            HW::FramebufferConfiguration{.Location = reinterpret_cast<void *>(0xFFFFA00000000000 + videoMode.FrameBufferPhysicalAddress), // TODO
                                         .ScreenWidth = videoMode.Width,
                                         .ScreenHeight = videoMode.Height,
                                         .BPS = videoMode.BytesPerScanline,
                                         .BPP = static_cast<uint32_t>(videoMode.BitsPerPixel / 8),
                                         .RedPosition = videoMode.RedPosition,
                                         .GreenPosition = videoMode.GreenPosition,
                                         .BluePosition = videoMode.BluePosition};

        m_screenManager.InitializeWith(*parameters.Vbe.InfoBlock, Stdlib::Move(videoModes), Stdlib::Move(edid),
                                       framebufferConfig, fonts.Data);

        // Setup logging
        m_logManager.EnableOnscreenLogging(
            MakeRef<TerminalManager>(StaticRefCast<ITerminalInterface>(m_screenManager.GetTextInterface())));

        // Some info
        FK_LOG_DEBUG("We have kernel logger!");
        FK_LOG_DEBUG_F("screen size in character %dx%d", m_screenManager.GetTextInterface()->GetScreenWidth(),
                       m_screenManager.GetTextInterface()->GetScreenHeight());
        FK_LOG_DEBUG("Hello world from video mode :woah:");

        for (;;) {
            HW::CPU::Halt();
        }
        F_NO_RETURN;
    }

    const Bootparams::BootDriveInfo& Kernel64::GetBootDriveInfo() const {
        return m_bootDriveInfo;
    }

    VMM::VirtualMemoryManager& Kernel64::GetVirtualMemoryManager() {
        return m_virtualMemoryManager;
    }

    Misc::MemoryAllocator::StaticMemoryAllocator& Kernel64::GetKernelAllocator() {
        return m_kernelAllocator;
    }

    LogManager& Kernel64::GetLogManager() {
        return m_logManager;
    }

    GFX::ScreenManager& Kernel64::GetScreenManager() {
        return m_screenManager;
    }

    Kernel64::Kernel64() = default;

}  // namespace FunnyOS::Kernel
