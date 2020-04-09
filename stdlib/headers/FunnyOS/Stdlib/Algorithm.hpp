#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP

namespace FunnyOS::Stdlib {

    /**
     * Returns a lower value from the two supplied.
     * @tparam T type of the values
     * @param[in] a first value to test
     * @param[in] b second value to test
     * @return lowest of those values
     */
    template <typename T>
    [[nodiscard]] constexpr const T& Min(const T& a, const T& b) noexcept(noexcept(a < b));

    /**
     * Returns a higher value from the two supplied.
     * @tparam T type of the values
     * @param[in] a first value to test
     * @param[in] b second value to test
     * @return higher of those values
     */
    template <typename T>
    [[nodiscard]] constexpr const T& Max(const T& a, const T& b) noexcept(noexcept(a > b));

}  // namespace FunnyOS::Stdlib

#include "Algorithm.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP
