#include <FunnyOS/Kernel/Kernel.hpp>

#include <FunnyOS/Stdlib/Config.hpp>
#include <FunnyOS/Stdlib/Logging.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Hardware/Serial.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManagerLoggingSink.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

extern void* KERNEL_HEAP;
extern void* KERNEL_HEAP_TOP;

namespace FunnyOS::Kernel {
    Kernel64& Kernel64::Get() {
        static Kernel64 c_instance;
        return c_instance;
    }

    class TestSink : public Stdlib::ILoggingSink {
       private:
        void Write(const char* message) {
            while (*message) {
                while (!CanWrite(HW::Serial::COMPort::COM1))
                    ;
                HW::Serial::Write(HW::Serial::COMPort::COM1, *message);
                message++;
            }
        }

       public:
        void SubmitMessage(Stdlib::LogLevel level, const char* message) override {
            Write(GetLogLevelName(level));
            Write(": ");
            Write(message);
            Write("\r\n");
        }
    };

    [[noreturn]] void Kernel64::Main(Bootparams::Parameters& parameters) {
        using namespace Misc::TerminalManager;

        FK_PANIC_IF(m_initialized, "Kernel already initialized");

        m_initialized = true;

        // Setup boot parameters
        m_bootDriveInfo = parameters.BootInfo;

        // Initialize allocator
        using Misc::MemoryAllocator::memoryaddress_t;
        m_kernelAllocator.Initialize(
            reinterpret_cast<memoryaddress_t>(&KERNEL_HEAP), reinterpret_cast<memoryaddress_t>(&KERNEL_HEAP_TOP));

        // Load kernel GDT
        // Null selector
        m_kernelGdt[0] = 0;

        // Kernel data
        m_kernelGdt[GDT_SELECTOR_DATA] =
            HW::CreateGdtDescriptor({.BaseAddress = 0, .Limit = 0, .Type = HW::GDTEntryType::Data, .IsPresent = true});

        // Kernel code
        m_kernelGdt[GDT_SELECTOR_CODE_RING0] = HW::CreateGdtDescriptor(
            {.BaseAddress              = 0,
             .Limit                    = 0,
             .Type                     = HW::GDTEntryType::Code,
             .IsPresent                = true,
             .DescriptorPrivilegeLevel = 0,
             .IsConforming             = true,
             .IsLongMode               = true,
             .Is32Bit                  = false});

        HW::LoadGdt({m_kernelGdt, F_SIZEOF_BUFFER(m_kernelGdt)});
        HW::LoadNewSegments(GDT_SELECTOR_CODE_RING0, GDT_SELECTOR_DATA);

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
            parameters.Vbe.EdidBlock == nullptr
                ? Stdlib::EmptyOptional<Bootparams::EdidInformation>()
                : Stdlib::MakeOptional<Bootparams::EdidInformation>(*parameters.Vbe.EdidBlock);

        // Copy fonts
        auto fonts = Stdlib::Memory::AllocateBuffer<uint8_t>(parameters.BiosFontsSize);
        Stdlib::Memory::Copy(fonts, static_cast<uint8_t*>(parameters.BiosFonts));

        const auto& videoMode = parameters.Vbe.ModeInfoStart[parameters.Vbe.ActiveModeIndex];

        auto framebufferConfig = HW::FramebufferConfiguration{
            .Location      = MM::PhysicalAddressToPointer(videoMode.FrameBufferPhysicalAddress),
            .ScreenWidth   = videoMode.Width,
            .ScreenHeight  = videoMode.Height,
            .BPS           = videoMode.BytesPerScanline,
            .BPP           = static_cast<uint32_t>(videoMode.BitsPerPixel / 8),
            .RedPosition   = videoMode.RedPosition,
            .GreenPosition = videoMode.GreenPosition,
            .BluePosition  = videoMode.BluePosition};

        m_screenManager.InitializeWith(
            *parameters.Vbe.InfoBlock, Stdlib::Move(videoModes), Stdlib::Move(edid), framebufferConfig, fonts.Data);
        m_logManager.GetLogger().AddSink(StaticRefCast<Stdlib::ILoggingSink>(Stdlib::MakeRef<TestSink>()));

        // Setup logging
        m_logManager.EnableOnscreenLogging(
            MakeRef<TerminalManager>(StaticRefCast<ITerminalInterface>(m_screenManager.GetTextInterface())));

        // Some info
        FK_LOG_DEBUG("We have kernel logger!");
        FK_LOG_DEBUG_F(
            "Screen size in character %dx%d", m_screenManager.GetTextInterface()->GetScreenWidth(),
            m_screenManager.GetTextInterface()->GetScreenHeight());

        FK_LOG_INFO("FunnyOS Kernel starting. Version: " FUNNYOS_VERSION);

        // Setup memory management
        m_physicalMemoryManager.Initialize(parameters.MemoryMap);
        m_virtualMemoryManager.InitializePageTables();
        m_physicalMemoryManager.ReclaimMemory(Bootparams::MemoryMapEntryType::PageTableReclaimable);
        m_physicalMemoryManager.ReclaimMemory(Bootparams::MemoryMapEntryType::LongMemReclaimable);

        FK_LOG_OK("Kernel initialized.");

        FK_PANIC("kekw");

        for (;;) {
            HW::CPU::Halt();
        }
        F_NO_RETURN;
    }

    void Kernel64::Panic(InterruptFrame* frame, const char* detail) {
        // TODO: If logging is not supported at this point fall back to VGA

        FK_LOG_FATAL("Kernel panic!");
        FK_LOG_FATAL_F("\t%s", detail);

        if (frame == nullptr) {
            FK_LOG_FATAL("\tNo interrupt frame");
        } else {
            // TODO
        }

        HW::DisableHardwareInterrupts();

        for (;;) {
            HW::CPU::Halt();
        }

        F_NO_RETURN;
    }

    const Bootparams::BootDriveInfo& Kernel64::GetBootDriveInfo() const {
        return m_bootDriveInfo;
    }

    MM::PhysicalMemoryManager& Kernel64::GetPhysicalMemoryManager() {
        return m_physicalMemoryManager;
    }

    MM::VirtualMemoryManager& Kernel64::GetVirtualMemoryManager() {
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

#pragma clang diagnostic pop