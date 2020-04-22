#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_COMPILER_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_COMPILER_HPP

#ifdef __GNUC__

// Function-related
#   define F_ALWAYS_INLINE                      __attribute__((always_inline))
#   define F_NEVER_INLINE                       __attribute__((noinline))
#   define F_CDECL                              __attribute__((__cdecl__))
#   define F_SECTION(sectionName)               __attribute__((section(sectionName)))

// Struct alignment
#   define F_DONT_ALIGN                         __attribute__((packed))

// Debugging
#   define F_UNIVERSAL_DEBUGGER_TRAP asm volatile ("xchg bx, bx")

// Varags
#   define va_list      __builtin_va_list
#   define va_start     __builtin_va_start
#   define va_arg       __builtin_va_arg
#   define va_copy      __builtin_va_copy
#   define va_end       __builtin_va_end


// Misc
#   define F_FETCH_CALLER_ADDRESS()             (static_cast<void*>(__builtin_return_address(0)))
#   define _F_TO_STRING_HELPER(x) #x
#   define F_TO_STRING(x) _F_TO_STRING_HELPER(x)

#else
#   error "Unsupported compiler"
#endif

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_COMPILER_HPP
