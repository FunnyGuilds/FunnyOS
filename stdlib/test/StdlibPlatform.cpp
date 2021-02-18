#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-no-malloc"
#pragma ide diagnostic ignored "hicpp-no-malloc"

#include "Common.hpp"

#include <memory>
#include <cstring>
#include <malloc.h>
#include <FunnyOS/Stdlib/Platform.hpp>

namespace FunnyOS::_Platform {
    using namespace FunnyOS::Stdlib;

    void* AllocateMemoryAligned(size_t size, size_t aligned) noexcept {
        return aligned_alloc(aligned, size);
    }

    void* ReallocateMemoryAligned(void* memory, size_t size, size_t alignment) noexcept {
        void* newMem = AllocateMemoryAligned(size, alignment);
        if (newMem == nullptr) {
            return nullptr;
        }

        memcpy(newMem, memory, std::min(size, malloc_usable_size(memory)));
        free(memory);
        return newMem;
    }

    void FreeMemory(void* memory) noexcept {
        return free(memory);
    }

    void ReportError(const char* error) noexcept {
        fprintf(stderr, "%s", error);
    }

    void Terminate(const char* error) noexcept {
        ReportError(error);
        std::terminate();
    }

}  // namespace FunnyOS::_Platform

#pragma clang diagnostic pop