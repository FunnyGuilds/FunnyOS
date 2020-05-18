#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MATH_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MATH_HPP

#include "TypeTraits.hpp"
#include "IntegerTypes.hpp"

namespace FunnyOS::Stdlib::Math {
    /**
     * Divides two integers [x] and [y] and rounds the result up.
     *
     * @tparam T type
     * @param x dividend
     * @param y divisor
     * @return result, rounded up
     */
    template <typename T, typename = EnableIf<NumeralTraits::IsInteger<T>>>
    T DivideRoundUp(T x, T y);

}  // namespace FunnyOS::Stdlib::Math

#include "Math.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MATH_HPP
