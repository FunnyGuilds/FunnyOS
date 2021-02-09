#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include "Bootloader.hpp"

/**
 * Platform-specific function implementations for the Stdlib
 */
namespace FunnyOS::_Platform {
    using namespace FunnyOS::Bootloader64;

    void* AllocateMemoryAligned(size_t size, size_t aligned) noexcept {
        return Bootloader::Get().GetAllocator().Allocate(size, aligned);
    }

    void* ReallocateMemoryAligned(void* memory, size_t size, size_t alignment) noexcept {
        return Bootloader::Get().GetAllocator().Reallocate(memory, size, alignment);
    }

    void FreeMemory(void* memory) noexcept {
        return Bootloader::Get().GetAllocator().Free(memory);
    }

    void ReportError(const char* error) noexcept {
        FB_LOG_ERROR(error);
    }

    void Terminate(const char* error) noexcept {
        ReportError(error);
// TODO        Bootloader::Get().Panic(error);
        F_NO_RETURN;
    }

}  // namespace FunnyOS::_Platform
