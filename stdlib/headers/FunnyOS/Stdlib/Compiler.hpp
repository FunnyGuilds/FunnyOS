#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_COMPILER_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_COMPILER_HPP

#ifdef __GNUC__

#   define FOS_NEVER_INLINE __attribute__((noinline))

#   define F_FETCH_CALLER_ADDRESS() (static_cast<void*>(__builtin_return_address(0)))

// Struct alignment
#   define F_DONT_ALIGN __attribute__((packed))

// Debugging
#   define F_UNIVERSAL_DEBUGGER_TRAP __asm__ __volatile__ ("xchg bx, bx")

// Varags
#   define va_list      __builtin_va_list
#   define va_start     __builtin_va_start
#   define va_arg       __builtin_va_arg
#   define va_copy      __builtin_va_copy
#   define va_end       __builtin_va_end

#else
#   error "Unsupported compiler"
#endif

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_COMPILER_HPP
