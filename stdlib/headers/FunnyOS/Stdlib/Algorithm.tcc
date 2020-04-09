#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP
#error "Include Algorithm.hpp instead"
#endif
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_TCC

namespace FunnyOS::Stdlib {

    template <typename T>
    constexpr const T& Min(const T& a, const T& b) noexcept(noexcept(a < b)) {
        return a < b ? a : b;
    }

    template <typename T>
    constexpr const T& Max(const T& a, const T& b) noexcept(noexcept(a > b)) {
        return a > b ? a : b;
    }
}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_TCC
