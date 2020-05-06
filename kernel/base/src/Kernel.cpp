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