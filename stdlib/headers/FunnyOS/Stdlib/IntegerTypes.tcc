#ifndef FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
#   error "Include IntegerTypes.hpp instead"
#endif
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INTEGERTYPES_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INTEGERTYPES_TCC
#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-runtime-int"

#include "TypeTraits.hpp"

namespace FunnyOS::Stdlib::NumeralTypes::Traits::Detail {
            template <typename T> struct IsInteger : TypeTraits::False {};

            template <> struct IsInteger<char              > : TypeTraits::True {};
            template <> struct IsInteger<short             > : TypeTraits::True {};
            template <> struct IsInteger<int               > : TypeTraits::True {};
            template <> struct IsInteger<long              > : TypeTraits::True {};
            template <> struct IsInteger<unsigned char     > : TypeTraits::True {};
            template <> struct IsInteger<unsigned short    > : TypeTraits::True {};
            template <> struct IsInteger<unsigned int      > : TypeTraits::True {};
            template <> struct IsInteger<unsigned long     > : TypeTraits::True {};
            template <> struct IsInteger<long long         > : TypeTraits::True {};
            template <> struct IsInteger<unsigned long long> : TypeTraits::True {};

            template <typename T> struct IsFloatingPoint : TypeTraits::False {};

            template <> struct IsFloatingPoint<float      > : TypeTraits::True {};
            template <> struct IsFloatingPoint<double     > : TypeTraits::True {};
            template <> struct IsFloatingPoint<long double> : TypeTraits::True {};

            template <typename T>
            struct IsNumber
                : TypeTraits::HasBooleanValue<IsInteger<T>::Value || IsFloatingPoint<T>::Value> {};
        }  // namespace FunnyOS::Stdlib::NumeralTypes

#pragma clang diagnostic pop
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INTEGERTYPES_TCC
