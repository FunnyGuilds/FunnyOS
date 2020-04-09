#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/BootloaderCommons/Logging.hpp>
#include <FunnyOS/BootloaderCommons/Bootloader.hpp>

/**
 * Platform-specific function implementations for the Stdlib
 */
namespace FunnyOS::_Platform {
    using namespace FunnyOS::Stdlib;

    void* AllocateMemoryAligned(size_t size, size_t /*aligned*/) {
        // We don't really support alignments in bootloader, let's hope nobody's gonna be angry about this.
        return Bootloader::GetBootloader()->GetAllocator().Allocate(size);
    }

    void* ReallocateMemory(void* memory, size_t size) {
        return Bootloader::GetBootloader()->GetAllocator().Reallocate(memory, size);
    }

    void FreeMemory(void* memory) {
        return Bootloader::GetBootloader()->GetAllocator().Free(memory);
    }

    void ReportError(const char* error) {
        FB_LOG_ERROR(error);
    }

    void Terminate(const char* error) {
        FunnyOS::Bootloader::GetBootloader()->Panic(error);
    }

}  // namespace FunnyOS::_Platform