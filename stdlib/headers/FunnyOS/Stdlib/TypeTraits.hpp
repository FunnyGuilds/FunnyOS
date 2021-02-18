#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_TYPETRAITS_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_TYPETRAITS_HPP

// clang-format off
namespace FunnyOS::Stdlib {

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

    /**
     * Represents a type that has a 'type' value.
     *
     * @tparam T the type
     */
    template <typename T> struct HasType { using Type = T; };

    namespace Detail {
        template <bool C, typename T, typename F> struct Conditional              : HasType<T> {};
        template <        typename T, typename F> struct Conditional<false, T, F> : HasType<F> {};
    }  // namespace Detail

    /**
     * Conditional type, equal to [T] if [C] evaluates to true, or to [F] if [C] evaluates to false.
     *
     * @tparam C condition to evaluate
     * @tparam T type if C is equal true
     * @tparam F type if F is equal false
     */
    template <bool C, typename T, typename F>
    using Conditional = typename Detail::Conditional<C, T, F>::Type;

    namespace Detail {
        template <bool Expression, typename T = void> struct EnableIf {};

        template <typename T> struct EnableIf<true, T> { using type = T; };
    }  // namespace Detail

    /**
     * An expression that is only valid valid whe [Expression] type parameter is true.
     */
    template <bool Expression, typename T = void> using EnableIf = typename Detail::EnableIf<Expression, T>::type;

    namespace Detail {
        template <typename A, typename B>  struct IsSame         : False {};
        template <typename A>              struct IsSame<A, A>   : True {};
    }  // namespace Detail

    /**
     * An expression that is 'true' only when A and B are equal.
     * @tparam A value to check
     * @tparam B value to check
     */
    template <typename A, typename B> constexpr bool IsSame = Detail::IsSame<A, B>::Value;

    namespace Detail {
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
    template<typename T> using RemoveVolatile = typename Detail::RemoveVolatile<T>::Type;

    /**
     * Removes a 'const' specifier from a type.
     */
    template<typename T> using RemoveConst = typename Detail::RemoveConst<T>::Type;

    /**
     * Removes both 'const' and 'volatile' specifiers from a type.
     */
    template<typename T> using RemoveCV = typename Detail::RemoveCV<T>::Type;

    namespace Detail {
        template <typename T> struct IsConst          : False {};
        template <typename T> struct IsConst<const T> : True {};

        template <typename T> struct IsVolatile             : False {};
        template <typename T> struct IsVolatile<volatile T> : True {};
    }

    /**
     * Checks if the given type [T] is 'const' qualified
     *
     * @param T type to check
     */
    template<typename T>
    constexpr bool IsConst = Detail::IsConst<T>::Value;

    /**
     * Checks if the given type [T] is 'volatile' qualified
     *
     * @param T type to check
     */
    template<typename T>
    constexpr bool IsVolatile = Detail::IsVolatile<T>::Value;

    namespace Detail {
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
    template <typename T> using RemoveReference = typename Detail::RemoveReference<T>::Type;

    /**
     * Check if T is void, possibly CV-qualified
     *
     * @param T type to check
     */
     template <typename T>
    constexpr bool IsVoid = IsSame<RemoveCV<T>, void>;

}  // namespace FunnyOS::Stdlib
// clang-format on

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_TYPETRAITS_HPP
