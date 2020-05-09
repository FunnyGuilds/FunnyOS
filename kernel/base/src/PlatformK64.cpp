#include <FunnyOS/Stdlib/IntegerTypes.hpp>

#include <FunnyOS/Kernel/Kernel.hpp>

/**
 * Platform-specific function implementations for the kernel Stdlib
 */
namespace FunnyOS::_Platform {
    using namespace FunnyOS::Stdlib;

    // TODO
    void* AllocateMemoryAligned(size_t size, size_t aligned) noexcept {
        return Kernel::Kernel64::Get().GetKernelAllocator().Allocate(size, aligned);
    }

    void* ReallocateMemoryAligned(void* memory, size_t size, size_t alignment) noexcept {
        return Kernel::Kernel64::Get().GetKernelAllocator().Reallocate(memory, size, alignment);
    }

    void FreeMemory(void* memory) noexcept {
        return Kernel::Kernel64::Get().GetKernelAllocator().Free(memory);
    }

    void ReportError(const char* error) noexcept {
        static_cast<void>(error);
    }

    void Terminate(const char* error) noexcept {
        ReportError(error);
        for (;;) {
            asm volatile(
                "cli\n"
                "hlt");
        }
    }

}  // namespace FunnyOS::_Platform