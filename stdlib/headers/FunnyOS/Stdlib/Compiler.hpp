#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_COMPILER_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_COMPILER_HPP
#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-deprecated-headers"
#pragma ide diagnostic ignored "hicpp-deprecated-headers"

// clang-format off
#ifdef __GNUC__

// Function-related
#   define F_ALWAYS_INLINE                      __attribute__((always_inline))
#   define F_NEVER_INLINE                       __attribute__((noinline))
#   define F_CDECL                              __attribute__((__cdecl__))
#   define F_SECTION(sectionName)               __attribute__((section(sectionName)))
#   define F_UNUSED                             __attribute__((unused))
#   define F_NAKED                              __attribute__((naked))

// Struct alignment
#   define F_DONT_ALIGN                         __attribute__((packed))

// Debugging
#   define F_UNIVERSAL_DEBUGGER_TRAP asm volatile ("xchg %bx, %bx")

// Varags
#include <stdarg.h>

// Misc
#   define F_FETCH_CALLER_ADDRESS()             (static_cast<void*>(__builtin_return_address(0)))

#   define _F_TO_STRING_HELPER(x) #x
#   define F_TO_STRING(x) _F_TO_STRING_HELPER(x)

#   define F_NO_RETURN                          __builtin_unreachable()

#else
#   error "Unsupported compiler"
#endif

#pragma clang diagnostic pop
// clang-format on

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_COMPILER_HPP
