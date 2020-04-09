#ifndef FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
#error "Include IntegerTypes.hpp instead"
#endif
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INTEGERTYPES_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INTEGERTYPES_TCC
#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-runtime-int"

#include "TypeTraits.hpp"

// clang-format off
namespace FunnyOS::Stdlib::NumeralTraits::Detail {
    template <typename T> struct IsInteger : False {};

    template <> struct IsInteger<char              > : True {};
    template <> struct IsInteger<short             > : True {};
    template <> struct IsInteger<int               > : True {};
    template <> struct IsInteger<long              > : True {};
    template <> struct IsInteger<unsigned char     > : True {};
    template <> struct IsInteger<unsigned short    > : True {};
    template <> struct IsInteger<unsigned int      > : True {};
    template <> struct IsInteger<unsigned long     > : True {};
    template <> struct IsInteger<long long         > : True {};
    template <> struct IsInteger<unsigned long long> : True {};

    template <typename T> struct IsFloatingPoint : False {};

    template <> struct IsFloatingPoint<float      > : True {};
    template <> struct IsFloatingPoint<double     > : True {};
    template <> struct IsFloatingPoint<long double> : True {};

    template <typename T>
    struct IsNumber : HasBooleanValue<IsInteger<T>::Value || IsFloatingPoint<T>::Value> {};

}  // namespace FunnyOS::Stdlib::NumeralTraits::Detail
// clang-format on

#pragma clang diagnostic pop
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INTEGERTYPES_TCC
