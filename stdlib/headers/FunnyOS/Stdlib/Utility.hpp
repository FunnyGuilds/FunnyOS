#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_UTILITY_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_UTILITY_HPP

#include "TypeTraits.hpp"

// clang-format off
namespace FunnyOS::Stdlib {

    namespace Details {
        template <typename T> struct RemoveVolatile             : HasType<T> {};
        template <typename T> struct RemoveVolatile<volatile T> : HasType<T> {};

        template <typename T> struct RemoveConst                : HasType<T> {};
        template <typename T> struct RemoveConst<const T>       : HasType<T> {};

        template <typename T> struct RemoveCV                   : HasType<T> {};
        template <typename T> struct RemoveCV<const T         > : HasType<T> {};
        template <typename T> struct RemoveCV<volatile T      > : HasType<T> {};
        template <typename T> struct RemoveCV<const volatile T> : HasType<T> {};
    }  // namespace Details

    /**
     * Removes a 'volatile' specifier from a type.
     */
    template<typename T> using RemoveVolatile = typename Details::RemoveVolatile<T>::Type;

    /**
     * Removes a 'const' specifier from a type.
     */
    template<typename T> using RemoveConst = typename Details::RemoveConst<T>::Type;

    /**
     * Removes both 'cosnst' and 'volatile' specifiers from a type.
     */
    template<typename T> using RemoveCV = typename Details::RemoveCV<T>::Type;

    namespace Details {
        template <typename T> struct RemoveReference      : HasType<T> {};
        template <typename T> struct RemoveReference<T&>  : HasType<T> {};
        template <typename T> struct RemoveReference<T&&> : HasType<T> {};
    }  // namespace Details

    /**
     * If T is a reference to a type removes, RemoveReference removes it and returns T, if T is not a reference it is
     * returned as is.
     *
     * @tparam T type to remove reference from
     */
    template <typename T> using RemoveReference = typename Details::RemoveReference<T>::Type;

    /**
     * Casts the supplied argument to a rvalue reference type of T.
     *
     * @tparam T type of the argument
     * @param value the argument to cast
     * @return an rvalue reference to T
     */
    template<typename T>
    constexpr RemoveReference <T>&& Move(T&& value) noexcept {
        return static_cast<RemoveReference <T>&&>(value);
    }

    /**
     * Forwards an lvalue as either an lvalue or as an rvalue, depending on T.
     *
     * @tparam T type of the argument reference
     * @param value lvalue to forward
     * @return the forwarded value
     */
    template<typename T>
    constexpr T&& Forward(RemoveReference<T>& value) noexcept {
        return static_cast<T&&>(value);
    }

    /**
     * Forwards rvalues as rvalues and prohibits forwarding of rvalues as lvalues
     *
     * @tparam T type of the argument rvalue
     * @param value rvalue to forward
     * @return the forwarded value
     * @return
     */
    template<typename T>
    constexpr T&& Forward(T&& value) noexcept {
        return static_cast<T&&>(value);
    }

}  // namespace FunnyOS::Stdlib

// clang-format on
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_UTILITY_HPP
