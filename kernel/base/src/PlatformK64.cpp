#include <FunnyOS/Stdlib/IntegerTypes.hpp>

/**
 * Platform-specific function implementations for the kernel Stdlib
 */
namespace FunnyOS::_Platform {
    using namespace FunnyOS::Stdlib;

    // TODO
    void* AllocateMemoryAligned(size_t size, size_t aligned) noexcept {
        return nullptr;
    }

    void* ReallocateMemoryAligned(void* memory, size_t size, size_t alignment) noexcept {
        return nullptr;
    }

    void FreeMemory(void* memory) noexcept {}

    void ReportError(const char* error) noexcept {}

    void Terminate(const char* error) noexcept {
        ReportError(error);
    }

}  // namespace FunnyOS::_Platform