#include <FunnyOS/BootloaderCommons/Bootloader.hpp>

namespace FunnyOS::Bootloader {

    LowMemoryAllocator& BootloaderType::GetAllocator() {
        return m_allocator;
    }

}  // namespace FunnyOS::Bootloader