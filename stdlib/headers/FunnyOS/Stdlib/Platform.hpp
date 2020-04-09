#ifndef FUNNYOS_STDLIB_SRC_PLATFORM_HPP
#define FUNNYOS_STDLIB_SRC_PLATFORM_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

#ifdef F_STDLIB_PLATFORM_EXTERN
#define _EXTERN extern
#else
#define _EXTERN
#endif

/**
 * Platform-specific functions
 */
namespace FunnyOS::_Platform {

    [[nodiscard]] _EXTERN void* AllocateMemoryAligned(size_t size, size_t aligned);

    [[nodiscard]] _EXTERN void* ReallocateMemory(void* memory, size_t size);

    _EXTERN void FreeMemory(void* memory);

    _EXTERN void ReportError(const char* error);

    [[noreturn]] _EXTERN void Terminate(const char* error);

}  // namespace FunnyOS::_Platform

#undef _EXTERN
#endif  // FUNNYOS_STDLIB_SRC_PLATFORM_HPP
