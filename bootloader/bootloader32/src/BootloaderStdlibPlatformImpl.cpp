#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include "Logging.hpp"
#include "Bootloader32.hpp"

/**
 * Platform-specific function implementations for the Stdlib
 */
namespace FunnyOS::_Platform {
    using namespace FunnyOS::Stdlib;
    using namespace FunnyOS::Bootloader32;

    void* AllocateMemoryAligned(size_t size, size_t /*aligned*/) {
        // We don't really support alignments in bootloader, let's hope nobody's gonna be angry about this.
        return Bootloader::Get().GetAllocator().Allocate(size);
    }

    void* ReallocateMemory(void* memory, size_t size) {
        return Bootloader::Get().GetAllocator().Reallocate(memory, size);
    }

    void FreeMemory(void* memory) {
        return Bootloader::Get().GetAllocator().Free(memory);
    }

    void ReportError(const char* error) {
        FB_LOG_ERROR(error);
    }

    void Terminate(const char* error) {
        Bootloader::Get().Panic(error);
        for (;;) {
        }
    }

}  // namespace FunnyOS::_Platform