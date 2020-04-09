#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-no-malloc"

#include "Common.hpp"

#include <memory>

namespace FunnyOS::_Platform {
    using namespace FunnyOS::Stdlib;

    void* AllocateMemoryAligned(size_t size, size_t aligned) {
        return aligned_alloc(aligned, size);
    }

    void* ReallocateMemory(void* memory, size_t size) {
        return realloc(memory, size);
    }

    void FreeMemory(void* memory) {
        return free(memory);
    }

    void ReportError(const char* error) {
        fprintf(stderr, "%s", error);
    }

    void Terminate(const char* error) {
        ReportError(error);
        std::terminate();
    }

}  // namespace FunnyOS::_Platform

#pragma clang diagnostic pop