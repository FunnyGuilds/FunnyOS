#ifndef FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
#define FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
#include "TypeTraits.hpp"
#include "IntegerTypes.tcc"

#include <stddef.h>
#include <stdint.h>

namespace FunnyOS::Stdlib {
    // clang-format off
    using  int8_t       = :: int8_t;
    using  int16_t      = :: int16_t;
    using  int32_t      = :: int32_t;
    using  int64_t      = :: int64_t;
    using uint8_t       = ::uint8_t;
    using uint16_t      = ::uint16_t;
    using uint32_t      = ::uint32_t;
    using uint64_t      = ::uint64_t;

    using intmax_t   =  int64_t;
    using uintmax_t  = uint64_t;

    using  size_t    =  uintmax_t;
    using ssize_t    =   intmax_t;

    using  intptr_t   =  intmax_t;
    using uintptr_t   = uintmax_t;
    using ptrdiff_t  = intptr_t;

    namespace Detail {
        template <typename T> struct ToUnsigned {};
        template <typename T> struct ToSigned {};

        template<> struct ToUnsigned< int8_t > : HasType<uint8_t > {};
        template<> struct ToUnsigned< int16_t> : HasType<uint16_t> {};
        template<> struct ToUnsigned< int32_t> : HasType<uint32_t> {};
        template<> struct ToUnsigned< int64_t> : HasType<uint64_t> {};
        template<> struct ToUnsigned<uint8_t > : HasType<uint8_t > {};
        template<> struct ToUnsigned<uint16_t> : HasType<uint16_t> {};
        template<> struct ToUnsigned<uint32_t> : HasType<uint32_t> {};
        template<> struct ToUnsigned<uint64_t> : HasType<uint64_t> {};

        template<> struct ToSigned<uint8_t > : HasType<int8_t > {};
        template<> struct ToSigned<uint16_t> : HasType<int16_t> {};
        template<> struct ToSigned<uint32_t> : HasType<int32_t> {};
        template<> struct ToSigned<uint64_t> : HasType<int64_t> {};
        template<> struct ToSigned< int8_t > : HasType<int8_t > {};
        template<> struct ToSigned< int16_t> : HasType<int16_t> {};
        template<> struct ToSigned< int32_t> : HasType<int32_t> {};
        template<> struct ToSigned< int64_t> : HasType<int64_t> {};
    }  // namespace Detail

    /**
     * An unsigned equivalent of type T
     */
    template <typename T>
    using ToUnsigned = typename Detail::ToUnsigned<T>::Type;

    /**
     * A signed equivalent of type T
     */
    template <typename T>
    using ToSigned = typename Detail::ToSigned<T>::Type;

    namespace NumeralTraits {
        /**
         * Tests whether or not the given type is an integer.
         */
        template<typename Type>
        inline constexpr bool IsInteger = NumeralTraits::Detail::IsInteger<Type>::Value;

        /**
         * Tests whether or not the given type is an floating point number.
         */
        template<typename Type>
        inline constexpr bool IsFloatingPoint = NumeralTraits::Detail::IsFloatingPoint<Type>::Value;

        /**
         * Tests whether or not the given type is an integer or a floating point number.
        */
        template<typename Type>
        inline constexpr bool IsNumber = NumeralTraits::Detail::IsNumber<Type>::Value;

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Woverflow"

        /**
         * Struct containing information about an integer type.
         */
        template<typename Type>
        struct Info {
            static_assert(IsInteger<Type>, "Type must be an integer");

            /**
             * Whether or not the type is a signed type.
             */
            constexpr const static bool IsSigned    = Type(-1) < Type(0);

            /**
             * Whether or not the type is an unsigned type.
             */
            constexpr const static bool IsUnsigned  = !IsSigned;

            /**
             * Minimum value that the type can store
             */
            constexpr const static Type MinimumValue = IsSigned ? static_cast<Type>(1U << (sizeof(Type) * 8 - 1)) : 0;

            /**
             * Maximum value that the type can store.
             */
            constexpr const static Type MaximumValue = MinimumValue - 1;

            Info() = delete;
        };
        #pragma GCC diagnostic pop

    }  // namespace NumeralTraits
}  // namespace FunnyOS::Stdlib

#include "IntegerTypes.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
