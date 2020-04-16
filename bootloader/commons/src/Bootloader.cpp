#include <FunnyOS/BootloaderCommons/Bootloader.hpp>

namespace FunnyOS::Bootloader {

    void BootloaderType::Main() {
        const uintptr_t memoryMapStart = GetBootloaderParameters().MemoryMapStart;
        const uintptr_t memoryMapEnd = memoryMapStart + GetBootloaderParameters().MemoryMapEntriesCount *
                                                            sizeof(BootloaderParameters::MemoryMapEntry);

        m_allocator.Initialize(memoryMapEnd, 0x0007FFFF);
    }

    LowMemoryAllocator& BootloaderType::GetAllocator() {
        return m_allocator;
    }

}  // namespace FunnyOS::Bootloader