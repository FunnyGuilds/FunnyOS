#include <FunnyOS/Kernel/Kernel.hpp>

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Hardware/VGA.hpp>
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

        for (size_t x = 1; x < 255; x++) {
            for (size_t y = 1; y < 255; y++) {
                uint32_t color = x | y << 8 | (x % y) << 16;
                uint32_t pixel_offset = y * videoMode.BytesPerScanline + (x * (videoMode.BitsPerPixel / 8));
                *reinterpret_cast<uint32_t*>(videoMode.FrameBufferPhysicalAddress + pixel_offset) = color;
            }
        }

        for (;;) {
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