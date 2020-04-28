#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP
#error "Include Algorithm.hpp instead"
#endif
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_TCC

namespace FunnyOS::Stdlib {

    template <typename T>
    constexpr T Min(T a, T b) noexcept(noexcept(a < b)) {
        return a < b ? a : b;
    }

    template <typename T>
    constexpr T Max(T a, T b) noexcept(noexcept(a > b)) {
        return a > b ? a : b;
    }

    template <typename T, typename... Args>
    [[nodiscard]] constexpr T Min(T value1, T value2, Args... args) noexcept(noexcept(Min(value1, value2))) {
        T min = Min(value1, value2);

        if (sizeof...(Args) > 0) {
            return Min(min, args...);
        }

        return min;
    }

    template <typename T, typename... Args>
    [[nodiscard]] constexpr T Max(T value1, T value2, Args... args) noexcept(noexcept(Max(value1, value2))) {
        T max = Max(value1, value2);

        if (sizeof...(Args) > 0) {
            return Max(max, args...);
        }

        return max;
    }
}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_TCC
