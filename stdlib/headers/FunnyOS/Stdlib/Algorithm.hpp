#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP

namespace FunnyOS::Stdlib {

    /**
     * Returns a lower value from the two supplied.
     *
     * @tparam T type of the values
     * @param[in] a first value to test
     * @param[in] b second value to test
     * @return lowest of those values
     */
    template <typename T>
    [[nodiscard]] constexpr const T& Min(const T& a, const T& b) noexcept(noexcept(a < b));

    /**
     * Returns a higher value from the two supplied.
     *
     * @tparam T type of the values
     * @param[in] a first value to test
     * @param[in] b second value to test
     * @return higher of those values
     */
    template <typename T>
    [[nodiscard]] constexpr const T& Max(const T& a, const T& b) noexcept(noexcept(a > b));

    /**
     * Returns the lowest value from all of the supplied.
     *
     * @tparam T type of the values
     * @tparam Args type of the values
     * @param value1 first value to compare
     * @param value2 second value to compare
     * @param args rest of the values to compare
     * @return the lowest value
     */
    template <typename T, typename... Args>
    [[nodiscard]] constexpr const T& Min(T value1, T value2, Args... args);

    /**
     * Returns the highest value from all of the supplied.
     *
     * @tparam T type of the values
     * @tparam Args type of the values
     * @param value1 first value to compare
     * @param value2 second value to compare
     * @param args rest of the values to compare
     * @return the highest value
     */
    template <typename T, typename... Args>
    [[nodiscard]] constexpr const T& Max(T value1, T value2, Args... args);
}  // namespace FunnyOS::Stdlib

#include "Algorithm.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP
