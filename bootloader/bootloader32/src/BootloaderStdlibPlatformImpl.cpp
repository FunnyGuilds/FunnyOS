#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include "Logging.hpp"
#include "Bootloader32.hpp"

/**
 * Platform-specific function implementations for the Stdlib
 */
namespace FunnyOS::_Platform {
    using namespace FunnyOS::Stdlib;
    using namespace FunnyOS::Bootloader32;

    void* AllocateMemoryAligned(size_t size, size_t /*aligned*/) noexcept {
        // We don't really support alignments in bootloader, let's hope nobody's gonna be angry about this.
        return Bootloader::Get().GetAllocator().Allocate(size);
    }

    void* ReallocateMemory(void* memory, size_t size) noexcept {
        return Bootloader::Get().GetAllocator().Reallocate(memory, size);
    }

    void FreeMemory(void* memory) noexcept {
        return Bootloader::Get().GetAllocator().Free(memory);
    }

    void ReportError(const char* error) noexcept {
        FB_LOG_ERROR(error);
    }

    void Terminate(const char* error) noexcept {
        Bootloader::Get().Panic(error);
        for (;;) {
        }
    }

}  // namespace FunnyOS::_Platform