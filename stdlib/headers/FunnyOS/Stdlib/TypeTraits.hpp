#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_TYPETRAITS_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_TYPETRAITS_HPP

namespace FunnyOS::Stdlib::TypeTraits {

    namespace Detail {
        template <bool Expression, typename T = void> struct EnableIf {};

        template <typename T> struct EnableIf<true, T> { using type = T; };
    }  // namespace Detail

    /**
     * An expression that is only valid valid whe [Expression] type parameter is true.
     */
    template <bool Expression, typename T = void> using EnableIf = typename Detail::EnableIf<Expression, T>::type;

    /**
     * Represents a type that has a value.
     *
     * @tparam Type type of the value
     * @tparam Val value
     */
    template <typename Type, Type Val> struct HasValue { constexpr const static Type Value = Val; };

    /**
     * Represents a type that has a value of type boolean.
     *
     * @tparam Val boolean value
     */
    template <bool Val> struct HasBooleanValue : HasValue<bool, Val> {};

    /**
     * Represents a type that has a boolean value 'true'.
     */
    struct True : HasBooleanValue<true> {};

    /**
     * Represents a type that has a boolean value 'false'.
     */
    struct False : HasBooleanValue<false> {};

    namespace Detail {
        template <typename A, typename B> struct IsSame : False {};
        template <typename A> struct IsSame<A, A> : True {};
    }  // namespace Detail

    /**
     * An expression that is 'true' only when A and B are equal.
     * @tparam A value to check
     * @tparam B value to check
     */
    template <typename A, typename B> constexpr bool IsSame = Detail::IsSame<A, B>::Value;

}  // namespace FunnyOS::Stdlib::TypeTraits

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_TYPETRAITS_HPP
