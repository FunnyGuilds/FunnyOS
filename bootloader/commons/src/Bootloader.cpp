#include <FunnyOS/BootloaderCommons/Bootloader.hpp>

namespace FunnyOS::Bootloader {

    void BootloaderType::Main(const BootloaderParameters& args) {
        const uintptr_t memoryMapStart = args.MemoryMapStart;
        const uintptr_t memoryMapEnd =
            memoryMapStart + args.MemoryMapEntriesCount * sizeof(BootloaderParameters::MemoryMapEntry);

        m_allocator.Initialize(memoryMapEnd, 0x00007DFF);
    }

    LowMemoryAllocator& BootloaderType::GetAllocator() {
        return m_allocator;
    }

}  // namespace FunnyOS::Bootloader