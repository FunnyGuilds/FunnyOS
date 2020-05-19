#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MATH_HPP
#error "Include Math.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MATH_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MATH_TCC

namespace FunnyOS::Stdlib::Math {
    template <typename T, typename>
    T DivideRoundUp(T x, T y) {
        if (x == 0) {
            return 0;
        }

        return 1 + ((x - 1) / y);
    }
}  // namespace FunnyOS::Stdlib::Math

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MATH_TCC